/**
 * @file		fj_host_callback.c
 * @brief		Host CallBack APIs.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/*----------------------------------------------------------------------*/
/* Include Files														*/
/*----------------------------------------------------------------------*/
#include "ddim_typedef.h"
#include "debug.h"
#include "fj_host.h"
#include "user.h"
#include "user_utility_jpeg.h"
#include "category_parameter.h"
#include "user_streaming.h"

#ifdef CO_HOST_PROC
#include "button.h"
#ifdef CO_UI_PROC
#include "ui_proc_main.h"
#endif
#endif
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define CO_STILL_CB_API_CHECK_ENABLE
#ifdef CO_STILL_CB_API_CHECK_ENABLE
#include "fj_host_debug.h"
#define M_CB_MBA_PRINTF(arg)						(FJ_MBA_PRINTF (arg))
#else
#define M_CB_MBA_PRINTF(arg)
#endif

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* CallBack APIs														*/
/*----------------------------------------------------------------------*/
///////////////////////////////////////////////////////////////////////////////
// ARM IPCU
///////////////////////////////////////////////////////////////////////////////
/**
This function will be called when the IPCU sending with Async will be finished.
*/
VOID FJ_HOST_IPCUFinishCB(UCHAR id, UCHAR *data, UINT32 length)
{
	#ifdef CO_HOST_PROC
	#ifdef CO_UI_PROC
	UPRINTF("%s(%d)\n", __func__, id);
	if(id == 4){
		OS_User_Set_Flg( FID_LINUX_CMD, 0x1 );
	}
	#endif
	#endif
}

///////////////////////////////////////////////////////////////////////////////
// Preview / Capture
///////////////////////////////////////////////////////////////////////////////
/**
This function called in View state.
Host controls 3a parameter from this API.
*/
BOOL FJ_Host3AInitCB( UCHAR sro_id, UCHAR init_type )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "3AInitCB" ));
	return TRUE;
}

/**
This function called in View state.
Host controls AHIP initialize from this API.
*/
BOOL FJ_HostAhipInitCB( UCHAR demosaic_id, UINT32 input_width, UINT32 input_lines )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "AhipInitCB" ));
	return TRUE;
}

VOID FJ_HostRawScalerConfigCB( UCHAR demosaic_id, FJ_STILL_RAW_SCALER_CFG* raw_scaler_cfg )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "RawSclCfgCB" ));
}

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function called once in Video state once.<br>
Host initiates RAW scaler from this API.
@param[in]	demosaic_id : Demosaic instance ID.
@retval 	TRUE  : Success
@retval 	FALSE : Error
*/

VOID FJ_HostRawScalerStartCB( UCHAR demosaic_id )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "RawSclStartCB" ));
}

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function called once in Video state once.<br>
Host stops RAW scaler from this API.
@param[in]	demosaic_id : Demosaic instance ID.
@retval 	TRUE  : Success
@retval 	FALSE : Error
*/
VOID FJ_HostRawScalerStopCB( UCHAR demosaic_id )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "RawSclStopCB" ));
}

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function called once in Video state once.<br>
Host stops RAW scaler from this API.
@param[in]		demosaic_id : Demosaic instance ID.
@param[in,out]	err_info : Error information of RAW scaller.See @ref FJ_STILL_RAW_SCALER_ERR_INFO<br>
@retval 	TRUE  : Success
@retval 	FALSE : Error
*/

VOID FJ_HostRawScalerErrorCB( UCHAR demosaic_id, FJ_STILL_RAW_SCALER_ERR_INFO* err_info )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "RawSclErrCB" ));
}


/**
This API is set a information on RAW to Host.
 */
BOOL FJ_HostVideoRawManipulation( UCHAR inst_id, FJ_RAW_MANI_VIDEO mani_no, PSRAWINFO psInfo, USHORT* puwScalers )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "VideoRawMani %u %u", inst_id, mani_no ));
	return TRUE;
}

/**
This API is set a information on YUV to Host.
 */
VOID FJ_HostVideoYUVManipulation( UCHAR demosaic_id, UCHAR output_id, FJ_YUV_MANI_VIDEO mani_no, PSYUVINFO psInfo )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "VideoYUVMani %u %u", demosaic_id, mani_no ));
}

/**
This CallBack function is called in boot only once.
Sensor should be initialized regarding HW.
*/
BOOL FJ_HostSensorInitCB( VOID )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "SensorInitCB" ));
	return TRUE;
}

/**
This CallBack function is called when View mode starts.
Sensor should be initialized regarding SW, or should be resumed from standby/sleep.
*/
BOOL FJ_HostSensorStartCB( UCHAR sensor_id )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "SensorStartCB" ));
	return TRUE;
}

/**
This CallBack function called when Sensor should be standby.
*/
BOOL FJ_HostSensorStopCB( UCHAR sensor_id )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "SensorStopCB" ));
	return TRUE;
}

/**
This CallBack function called when Sensor mode should be changed.
*/
BOOL FJ_HostSensorModeChangeCB( UCHAR sensor_id )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "SensorModeChangeCB" ));
	return TRUE;
}

/**
This CallBack function is called when Sensor mode change is finished and 1st View image set to buffer.
*/
BOOL FJ_HostSensorModeChangeCompleteCB( UCHAR sensor_id )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "SensorModeChangeCompleteCB" ));
	return TRUE;
}

/**
This Call Back function is called when Sensor mode will be changed.
*/
BOOL FJ_Host3AWindowConfigCB( UCHAR sro_id )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "3AWindowConfigCB" ));
	return TRUE;
}

/**
The CallBack function for the Pre-CNR setup.
*/
BOOL FJ_HostVideoIqParamUpdateCB( UCHAR sen_id, UCHAR demosaic_id, ULLONG frame_no )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_START, "VideoIqParamUpdateCB" ));
	
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_END, "VideoIqParamUpdateCB" ));

	return TRUE;
}

/**
The CallBack function for the Pre-CNR setup.
*/
BOOL FJ_HostStillIqParamUpdateCB( UCHAR sen_id, UCHAR demosaic_id, ULLONG frame_no )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "StillIqParamUpdateCB" ));
	return TRUE;
}

/**
This CallBack function is Called by interruption of VD. 
Mounting of the function controlled by timing of VD is assumed. 
*/
BOOL FJ_HostVDCB( UCHAR sensor_id, ULLONG frame_no )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "VDCB" ));
	return TRUE;
}

// --- REMOVE_EIS BEGIN ---
/**
This callback function read gyro data.  
*/
BOOL FJ_HostEISReadGyroCB( UCHAR sensor_id, ULLONG frame_no )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "EISReadGyroCB" ));
	return TRUE;
}

/**
This callback function get exposure time of the frame.  
*/
BOOL FJ_HostEISGetExposureTimeCB( UCHAR sensor_id, ULLONG frame_no, UINT32* exposure_time )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "EISGetExposureTimeCB%lu", (ULONG)frame_no ));
	return TRUE;
}
// --- REMOVE_EIS END ---

/**
This callback function get gyro output data.  
*/
BOOL FJ_HostEISGetGyroOutputCB( ULLONG frame_no )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "EISReadGyroCB" ));
	return TRUE;
}
// --- REMOVE_AHIP BEGIN ---
/**
This callback function is called in interruption handler of AHIP.
*/
VOID FJ_HostAHIPIntCB( VOID )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "AHIPIntCB" ));
}
// --- REMOVE_AHIP END ---

/**
It is called in interruption handler, which IPCUINT0 of DSP Peripheral Block occurs.
*/
VOID FJ_HostDspIPCU0IntCB( VOID )
{
//	DspPeri_IPCU_Int_Handler( D_IPCU_INT0_CPU );  // Interrupt Handler for INTID 269
}

/**
It is called in interruption handler, which IPCUINT1 of DSP Peripheral Block occurs.
*/
VOID FJ_HostDspIPCU1IntCB( VOID )
{
//	DspPeri_IPCU_Int_Handler( D_IPCU_INT1_CPU );  // Interrupt Handler for INTID 270
}

/**
It is called in interruption handler, which IPCUINT2 of DSP Peripheral Block occurs.
*/
VOID FJ_HostDspIPCU2IntCB( VOID )
{
//	DspPeri_IPCU_Int_Handler( D_IPCU_INT2_CPU );  // Interrupt Handler for INTID 271
}

/**
It is called in interruption handler, which IPCUINT3 of DSP Peripheral Block occurs.
*/
VOID FJ_HostDspIPCU3IntCB( VOID )
{
//	DspPeri_IPCU_Int_Handler( D_IPCU_INT3_CPU );  // Interrupt Handler for INTID 272
}

/**
This function set LPF parameter.
@param[in]	frame_no : Frame number.
@param[in]	stream_idx : Stream number.
@param[in]	img_info : Input image information. (ex. width, lines, etc...).
@param[out]	kernel_param : LPF setting parameter.
@retval		TRUE  : Success
@retval		FALSE : Error
*/
BOOL FJ_HostPreLpfCB( ULLONG frame_no, UINT32 stream_idx, FJ_IMG_INFO* img_info, FJ_IMG_SPF_KERNEL* kernel_param )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "PreLpfCB" ));
	return TRUE;
}

/* --- Still Capture --- */
/**
Function that passed information on RAW data frame.
 */
VOID FJ_HostStillRawManipulation( UCHAR inst_id, FJ_RAW_MANI_STILL mani_no, PSRAWINFO psInfo, USHORT* puwScalers )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "StillRawMani %u %u", inst_id, mani_no ));
}

/**
Function that passed information on YUV
 */
CHAR FJ_HostStillYUVManipulation( UCHAR demosaic_id, UCHAR output_id, FJ_YUV_MANI_STILL mani_no, PSYUVINFO psInfo )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "StillYUVMani %u %u", demosaic_id, mani_no ));
	return TRUE;
}

CHAR FJ_HostStitchYUVManipulation( UCHAR stitch_id, PSSTITCHINFO psInfo )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "StitchYUVMani %d", stitch_id ));
	return TRUE;
}

/**
Function that raw data format is converted to dng data format.
 */
UINT32 FJ_HostStillRaw2Dng( FJ_STILL_RAW_DATA_INFO* dcf_raw_data, FJ_STILL_RAW_DATA_INFO* dcf_dng_data )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "StillRaw2Dng" ));
	return TRUE;
}

UINT32 FJ_HostStitchSavePIVImg( UCHAR encode_id, ULLONG frame_no, FJ_IMG_INFO piv_img_info )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "StitchSavePIVImg %u %u", encode_id, (UINT32)frame_no ));
	return TRUE;
}

CHAR FJ_HostSHDRYUVManipulation( UCHAR demosaic_id, UCHAR output_id, FJ_YUV_MANI_SHDR mani_no, PSYUVINFO psInfo )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "SHDRYUVMani %u %u", demosaic_id, mani_no ));
	return TRUE;
}

CHAR FJ_HostSHDRRGBManipulation( UCHAR demosaic_id, UCHAR output_id, FJ_RGB_MANI_SHDR mani_no, PSRGBINFO psInfo )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "SHDRRGBMani %u %u", demosaic_id, mani_no ));
	return TRUE;
}

/**
Function that passes jpeg data and information on YUV data frame
*/
UINT32 FJ_HostVideoJpgManipulation( UCHAR encode_id, FJ_JPG_MANI_VIDEO mani_no, PSJPGINFO psJpgInfo )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "VideoJpgMani %u %u", encode_id, mani_no ));
	return TRUE;
}

/**
Function that passes jpeg data and information on YUV data frame
*/
UINT32 FJ_HostStillJpgManipulation( UCHAR encode_id, FJ_JPG_MANI_STILL mani_no, PSJPGINFO psJpgInfo )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "StillJpgMani %u %u", encode_id, mani_no ));
	return TRUE;
}

/**
This Call Back function is called in Still Sequence BEFORE the next still in the sequence
*/
BOOL FJ_HostStillSequenceCB( UCHAR sen_id, UINT32 still_index )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "StillSequenceCB" ));
	return TRUE;
}

// --- REMOVE_PANO BEGIN ---
/**
This API is set a information on YUV to Host.
*/
BOOL FJ_HostPanoramaManipulation( PSPANOINFO psInfo )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "PanoramaManipulation" ));
	return TRUE;
}
// --- REMOVE_PANO END ---

/**
This CallBack function is Called to the timing which changes from exposure of Still processing to Readout. 
Inside this CallBack, it assumes that FJ_SgStart() is Called.
*/
BOOL FJ_HostStillExposureEndCB( UCHAR sen_id )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "StillExposureEndCB" ));
	return TRUE;
}

/**
This CallBack function is Readout of Still processing, and whenever Readout of each Field is completed, it is Called. 
*/
BOOL FJ_HostStillReadCB( UCHAR sen_id, UCHAR field_num )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "StillReadCB" ));
	return TRUE;
}

/**
This CallBack function is called after "StillYUVManipulation(Post-WARP)" when HCE want to wait User Confirm.
*/
VOID FJ_Host_UserConfirmCB( VOID )
{
	
}

/**
This CallBack function
*/
USHORT FJ_HostExposureCB( UCHAR sensor_id, UCHAR type, ULLONG frame_no )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "ExposureCB" ));
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 3A(AE)
///////////////////////////////////////////////////////////////////////////////
/**
This Callback function is called whenever Base FW meets the 'e' slot in statistics sequence, 
defined by Host FW by API function FJ_SetStatisticPeriod().
Host FW shall implement this Callback function, for processing its AE Algorithm. 
Host AE algorithm is expected to call FJ_SetPreviewExposureTime 
and other related APIs to notice its AE calculation result to Base FW.
*/
BOOL FJ_HostVideoAECB( UCHAR aeawb_id, AAA_Common_Info* cmn_info, ULONG* AeDataTopAddr, ULONG AeWinBlockColorPixelCount, ULLONG frame_no )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "VideoAECB" ));
	return TRUE;
}

/**
This CallbackFunction is Called to the timing which changes from View to CaptureMode. 
Host FW shall implement this Callback function, for processing its AE Algorithm. 
Host AE algorithm is expected to call FJ_SetCaptureExposureTime()
and other related APIs to notice its AE calculation result to Base FW.
*/
BOOL FJ_HostStillAECB( UCHAR aeawb_id, AAA_Common_Info* cmn_info, ULONG* AeDataTopAddr, ULONG AeWinBlockColorPixelCount, ULLONG frame_no )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "StillAECB" ));
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 3A(AWB)
///////////////////////////////////////////////////////////////////////////////
/**
This Callback function is called whenever Base FW meets the 'w' slot in statistics sequence, 
defined by Host FW by API function FJ_SetStatisticPeriod().
Host FW shall implement this Callback function, for processing its AWB Algorithm.
Host AWB algorithm is expected to call FJ_AWBSetVideoGain().
*/
BOOL FJ_HostVideoAWBCB( UCHAR aeawb_id, AAA_Common_Info* cmn_info, ULONG* AwbDataTopAddr, ULLONG frame_no )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "VideoAWBCB" ));
	return TRUE;
}

/**
This CallbackFunction is Called during Capture processing. 
Host FW shall implement this Callback function, for processing its AWB Algorithm. 
*/
BOOL FJ_HostStillAWBCB( UCHAR aeawb_id, AAA_Common_Info*	cmn_info, ULONG* AwbDataTopAddr, ULLONG frame_no )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "StillAWBCB" ));
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 3A(AF)
///////////////////////////////////////////////////////////////////////////////
/**
This Callback function is called whenever Base FW meets the 'f' slot in statistics sequence, 
defined by Host FW by API function FJ_SetStatisticPeriod().
Host FW can use this call to synchronize the AF behavior.
*/
BOOL FJ_HostAFCB( UCHAR af_id, AAA_Common_Info* cmn_info, ULLONG frame_no )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "AFCB" ));
	return TRUE;
}

/**
This Callback function is called when AF statistics is finished and each 
AF statistics data address to the address which is specified in the input parameter.
Host FW shall implement this Callback function, for processing its AF algorithm.
*/
BOOL FJ_HostAFDataReadyCB( UCHAR af_id, ULLONG frame_no )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "AFDataReadyCB" ));
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 3A(FLCK)
///////////////////////////////////////////////////////////////////////////////
/**
This Callback function is called whenever Base FW meets the 'l' slot in statistics sequence, 
defined by Host FW by API function FJ_SetStatisticPeriod().
Host FW shall implement this Callback function, for processing its Flicker Algorithm.
*/
BOOL FJ_HostFLCKCB( UCHAR flck_id, AAA_Common_Info* cmn_info, ULONG* FlckDataTopAddr, ULLONG frame_no )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "FLCKCB" ));
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// STAT
///////////////////////////////////////////////////////////////////////////////
VOID FJ_HostVideoStatParamUpdateCB( UCHAR inst_id )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "V_STATUDCB" ));
}

VOID FJ_HostStillStatParamUpdateCB( UCHAR inst_id )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "S_STATUDCB" ));

}

///////////////////////////////////////////////////////////////////////////////
// Image Quality
///////////////////////////////////////////////////////////////////////////////
/**
This function will be called during replace Tone table and Gamma table.
*/
VOID FJ_HostNoticeChgToneGammaCB( UCHAR demosaic_id, T_FJ_HOST_TONE_GAMMA* tone_gamma_param )
{
	M_CB_MBA_PRINTF(( FJ_MBALOG_FUNC_ID_CB_CHECK, FJ_MBALOG_TYPE_POINT, "NoticeChgToneGammaCB" ));
}

///////////////////////////////////////////////////////////////////////////////
// PlayBack
///////////////////////////////////////////////////////////////////////////////
/**
This function get progress number of FileOperation (copy/delete/protect) functions.
*/
BOOL FJ_Host_NoticeProgressCB( USHORT cur_progress_num, USHORT total_num )
{
	return TRUE;
}

/**
This function get thumbnail error param of thumbnailindex function.
*/
BOOL FJ_Host_ThumbnailIndexCB( FJ_T_PLAYBACK_THUMBNAILINDEX_INFO* thumb_info, UCHAR thumb_total_count )
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// IP
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// BOOT
///////////////////////////////////////////////////////////////////////////////
/**
This is callback API.<br>
The Base F/W executes Booting Process of Host Side during boot procedure.<br>
*/
VOID FJ_HostPreBootProc( ULONG ulArgument )
{
	#ifdef CO_HOST_PROC
	switch(ulArgument){
		case 0:
			FJ_ButtonInit(10);
		break;
		case 1:
			FJ_ButtonStartSample();
		#ifdef CO_UI_PROC
			UI_Proc_Main_Init();
		#endif
		break;
		default:
		break;
	}
	#endif
}

/**
This is callback API.<br>
The Base F/W executes Host Main Process after boot operation.<br>
Using this thread for user procedure all their functions.<br>
*/
VOID FJ_HostProc( ULONG ulArgument )
{
	#ifdef CO_HOST_PROC
	ULONG uMsgID;
	ULONG uMsgPara;
	#ifdef CO_UI_PROC
	T_UI_PROC_MSG msg;
	#endif
	
	OS_User_Chg_Pri(0, 50);
	
	UPRINTF("Wait Boot Finished!\n");
	OS_User_Slp_Tsk();
	UPRINTF("Boot Finished!\n");
		
	#ifdef CO_UI_PROC
	UI_Proc_Main_Start();
	#endif
			
	while( 1 ) {
		FJ_HostGetMessage( &uMsgID, &uMsgPara );
	
		UPRINTF("+++ MsgID=%x, MsgPara=%x\n", (UINT32)uMsgID, (UINT32)uMsgPara);
	
		#ifdef CO_UI_PROC
		msg.msgid = uMsgID;
		msg.ulPara1 = uMsgPara;
		msg.vpPara2 = NULL;
		UI_Proc_Main_EventTrig(&msg);
		#endif
	}	
	#endif
		
	OS_User_Ext_Tsk();
}


///////////////////////////////////////////////////////////////////////////////
// AUDIO
///////////////////////////////////////////////////////////////////////////////

/**
This callback function called, when the pcm data was captured
*/
VOID FJ_Host_AudioCapturePlugIn0CB( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_PLUGIN0_CB_PARAM * param )
{
	User_Utility_Record_Audio_CapturePlugIn0CB(audio_capture_id, param);
}

/**
This callback function called, when the pcm data was captured
*/
VOID FJ_Host_AudioCapturePlugIn1CB( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_PLUGIN1_CB_PARAM * param )
{
	User_Utility_Record_Audio_CapturePlugIn1CB(audio_capture_id, param);
}

/**
This callback function called, when the pcm data was decoded
*/
VOID FJ_Host_AudioOutPlugInCB( UCHAR audio_out_id, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM * param )
{
	User_Utility_Playback_AudioOutPlugInCB( audio_out_id, param );
}

/**
This callback function called, when the WAV output operation is stoped
*/
VOID FJ_Host_AudioOut_StopWavCB( UCHAR audio_out_id, UCHAR stop_factor )
{
	User_Utility_Playback_StopWavCB(audio_out_id, stop_factor);
}

///////////////////////////////////////////////////////////////////////////////
// Media
///////////////////////////////////////////////////////////////////////////////
/**
This API is called in CMD11, notify the timming of changing voltage.
*/
BOOL FJ_HostSDChangeVoltageCB( UCHAR ch, UCHAR voltage )
{
	// when you return TRUE, SD Driver change voltage
	return FALSE;
}

/**
This callback function control the power supply of the SD card.
*/
BOOL FJ_HostSDPowerSupplyCB( UCHAR ch, UCHAR power )
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// DCF
///////////////////////////////////////////////////////////////////////////////

/**
This Call Back function is called just before a DCF object is created.
*/
BOOL FJ_HostDcfCreateFileCB( UINT32 uiFileType )
{
#ifdef CO_DCF_EXIF_ON // use sample source
	INT32 ercd;
	USHORT dirCnt;
	USHORT objCnt;
	USHORT lastDirNo;
	USHORT lastFileNo;
	CHAR dirName[10];
	CHAR cateDirName[8] = {0};
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

	if(0 != ercd) {
		return FALSE;
	}
	if(0 == dirCnt) {
		// No directory
		// set directory name
		Cate_Get_Dcf_Dir_Name(cateDirName);
		FJ_SetDCFFreeChars(1, FJ_DIR_SET_ONLY, cateDirName);
		lastDirNo = 100;
		lastFileNo = 1;
		goto skip1;
	}

	// search object
	for(;;) {
		// check number of objects
		ercd = FJ_DirectDcfDumpDirIAttr( DCF_DB_CURRENT_DIR, &objCnt, dirName, &date, &time );
		if(0 != ercd){
			return FALSE;
		}
		dirNo = (dirName[0]-'0')*100 + (dirName[1]-'0')*10 + (dirName[2]-'0');
		// set directory name
		FJ_SetDCFFreeChars(1, FJ_DIR_SET_ONLY, dirName+3);

		// get last file number
		ercd = FJ_GetDCFLastFileNumber(DCF_DB_CURRENT_DIR, &fileNo);
		if(0 != ercd){
			return FALSE;
		}
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
		if(0 != ercd){
			return FALSE;
		}
		if(dirCnt != current_dir_index) {
			// not last dir.
			// change next dir.  !!Update DB!!
			nextDirIndex = (dirCnt > current_dir_index) ? (current_dir_index+1) : 1;
			ercd = FJ_SetCurrentDirectory(nextDirIndex);
			if(0 != ercd){
				return FALSE;
			}
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
			Cate_Get_Dcf_Dir_Name(cateDirName);
			FJ_SetDCFFreeChars(1, FJ_DIR_SET_ONLY, cateDirName);
			lastDirNo = dirNo + 1;
			lastFileNo = 1;
			break;
		}
		else{

			Cate_Get_Dcf_Dir_Name(cateDirName);
			FJ_SetDCFFreeChars(1, FJ_DIR_SET_ONLY, cateDirName);

			// get latest directory number
			ercd = FJ_GetParamLong(FJ_PARA_DCF_LAST_KEY, &lastdirnum);
			if(0 != ercd){
				return FALSE;
			}
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
#endif // CO_DCF_EXIF_ON
	
	return TRUE;
}

/**
This function will be called after a DCF object is inserted into DCF Db.<br>
Set filtering information for captured image.
*/
BOOL FJ_Host_SetDcfDbFilterTypeCB(CHAR* full_file_path)
{
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// EXIF
///////////////////////////////////////////////////////////////////////////////
/**
This function be called before the EXIF information is saved into a file.<br>
And HOST updates EXIF tag.
*/
BOOL FJ_Host_ExifUpdateCB( VOID )
{
#ifdef CO_DCF_EXIF_ON
	return User_Utility_ExifUpdateCB();
#else
	return TRUE;
#endif
}

/**
This function be called before the EXIF information is saved into a file.<br>
And HOST create EXIF MakerNote to byBuffer.
*/
LONG FJ_Host_CreateMakerNoteCB(CHAR* byBuffer, INT32 Size, INT32 Offset, INT32* ParamArray, INT32 ParamNum)
{
#ifdef CO_DCF_EXIF_ON
	return User_Utility_CreateMakerNoteCB( byBuffer, Size, Offset, ParamArray, ParamNum );
#else
	return TRUE;
#endif
}

/**
This function is invoked each time the image is play-backed, and before it is actually displayed.<br>
And HOST parse EXIF MakerNote.
*/
LONG FJ_Host_ParseMakerNoteCB(CHAR* byBuffer, INT32 Size, INT32 Offset, INT32 ByteOrder, INT32* ParamArray, INT32 ParamNum)
{
	return 0;
}

/**
This function be called before the EXIF information is saved into a file.<br>
And HOST updates EXIF tag for class1 image of the multi picture format.
*/
BOOL FJ_Host_ExifUpdate_Class1_CB( VOID )
{
	return TRUE;
}

/**
This function be called before the EXIF information is saved into a file.<br>
And HOST updates EXIF tag for class2 image of the multi picture format.
*/
BOOL FJ_Host_ExifUpdate_Class2_CB( VOID )
{
	return TRUE;
}

/**
This function be called before the EXIF information is saved into a file.<br>
And HOST create EXIF MakerNote for class1 image of the multi picture format from byBuffer.
*/
LONG FJ_Host_CreateMakerNote_Class1_CB(CHAR* byBuffer, INT32 Size, INT32 Offset, INT32* ParamArray, INT32 ParamNum)
{
	return 0;
}

/**
This function be called before the EXIF information is saved into a file.<br>
And HOST create EXIF MakerNote for class2 image of the multi picture format from byBuffer.
*/
LONG FJ_Host_CreateMakerNote_Class2_CB(CHAR* byBuffer, INT32 Size, INT32 Offset, INT32* ParamArray, INT32 ParamNum)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MOVIE
///////////////////////////////////////////////////////////////////////////////

// Encoder

/**
This function is notified at the time of Encode start.
*/
BOOL FJ_Host_VideoEncoderStartCB( UCHAR video_enc_id )
{
	return TRUE;
}

/**
This function set H264/H.265 video stream information at the beginning of the GOP.
*/
BOOL FJ_Host_EncoderSequenceStartCB( UCHAR video_enc_id, INT32 *vop_cnt )
{
	switch(video_enc_id){
		case E_VIDEO_ENOCDE_ID_H264_CH0:
		case E_VIDEO_ENOCDE_ID_H264_CH1:
		case E_VIDEO_ENOCDE_ID_H264_CH2:
			User_Utility_Record_H264_SequenceStartCB( video_enc_id );
		break;
		case E_VIDEO_ENOCDE_ID_H265_CH0:
		case E_VIDEO_ENOCDE_ID_H265_CH1:
			User_Utility_Record_H265_SequenceStartCB( video_enc_id, vop_cnt );
		break;
	}
		
	return TRUE;
}

/**
This function is notified at the Repeat Frame has occurred.
*/
BOOL FJ_Host_EncoderRepeatFrameCB( UCHAR video_enc_id )
{
	return TRUE;
}

/**
This function get H264 video stream vector information.
*/
BOOL FJ_Host_H264VectorCB( UCHAR video_enc_id, T_FJ_MOVIE_H264_VECTOR_PARAM* info )
{
	return TRUE;
}

/**
This function get H264 stream information, and set H264 stream information if had been handled.
*/
BOOL FJ_Host_H264StreamCB( UCHAR video_enc_id, T_FJ_MOVIE_H264_STREAM_PARAM* stream_info)
{
	User_Utility_Record_H264_StreamCB( video_enc_id, stream_info ); // ret is OK/NG not TRUE/FALSE
	return TRUE;
}

// --- REMOVE_ES3_HARDWARE BEGIN ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
/**
This function get H265 video stream vector information.
*/
BOOL FJ_Host_H265VectorCB( UCHAR video_enc_id, T_FJ_MOVIE_H265_VECTOR_PARAM* info)
{
	return TRUE;
}
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE END ---

/**
This function get H265 stream information, and set H265 stream information if had been handled.
*/
BOOL FJ_Host_H265StreamCB( UCHAR video_enc_id, T_FJ_MOVIE_H265_STREAM_PARAM* stream_info)
{
	User_Utility_Record_H265_StreamCB( video_enc_id, stream_info );
	return TRUE;
}

/**
This function get PCM stream information, and set PCM stream information if had been handled.
*/
BOOL FJ_Host_PCMStreamCB( UCHAR audio_enc_id, T_FJ_AUDIO_PCM_STREAM_PARAM* info)
{
	User_Utility_Record_Audio_PCMStreamCB(audio_enc_id, info); // ret is OK/NG not TRUE/FALSE
	return TRUE;
}

/**
This function get AAC stream information, and set AAC stream information if had been handled.
*/
BOOL FJ_Host_AACStreamCB( UCHAR audio_enc_id, T_FJ_AUDIO_AAC_STREAM_PARAM* info)
{
	User_Utility_Record_Audio_AACStreamCB(audio_enc_id, info); // ret is OK/NG not TRUE/FALSE
	return TRUE;
}

// Decoder

BOOL FJ_Host_VideoPlayback_GetVideoStreamCB( E_FJ_VIDEO_DEC_ENUM codec )
{
	if (User_IsStreamingProcess()) {
#if defined(D_STREAMING_FROM_FILE)
		User_StreamingGetFrameASync(codec, D_DECODE_REQUEST_FRAME_NUM);
#endif
	} else {
		User_Utility_Playback_GetVideoStreamCB();
	}
	return TRUE;
}

BOOL FJ_Host_VideoPlayback_GetAudioStreamCB( VOID )
{
	User_Utility_Playback_GetAudioStreamCB();
	return TRUE;
}

BOOL FJ_Host_VideoPlayback_FirstFrameDisplayCB( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error )
{
	User_IsStreamingProcess() ?
			User_StreamingFirstFrameDisplayCB(decoder_status, display_pts, error) :
			User_Utility_Playback_FirstFrameDisplayCB(decoder_status, display_pts, error);

	return TRUE;
}

BOOL FJ_Host_VideoPlayback_VideoDecodeCB( E_FJ_VIDEO_DEC_ENUM codec, E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error )
{
	User_IsStreamingProcess() ?
			User_StreamingDecodeOneFrameDoneCB(codec, decoder_status, display_pts, error) :
			User_Utility_Playback_VideoDecodeCB(decoder_status, display_pts, error);

	return TRUE;
}

BOOL FJ_Host_VideoPlayback_AudioDecodeCB( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG decoded_pts, BOOL error )
{
	User_Utility_Playback_AudioDecodeCB(decoder_status, decoded_pts, error);
	return TRUE;
}

// Mux

BOOL FJ_Host_Mux_AudioFreeRequestCB( UCHAR aud_id, UINT32 free_index, BOOL last_free_flg )
{
	User_Utility_Record_Audio_Free_Index(aud_id, free_index, last_free_flg); // ret is OK/NG not TRUE/FALSE
	return TRUE;
}

// --- REMOVE_CAMERAIF BEGIN ---
VOID FJ_Host_Mux_VideoAddSampleCB( UINT32 code )
{
	User_Utility_Record_Mux_VideoAddSampleCB( code );
}
// --- REMOVE_CAMERAIF END ---

// Demux

BOOL FJ_Host_Demux_StreamReadCompleteCB( UINT32 file_id, E_FJ_DEMUX_CODEC codec, T_FJ_DEMUX_STREAM_PARAM* stream_param )
{
	return User_Utility_Playback_StreamReadCompleteCB( file_id, codec, stream_param );
}


BOOL FJ_Host_Demux_GetStreamAreaCB( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32* stream_size, UINT32* stream_address )
{
	return User_Utility_Playback_GetStreamAreaCB( file_id, codec, stream_size, stream_address );
}



///////////////////////////////////////////////////////////////////////////////
// Peripheral
///////////////////////////////////////////////////////////////////////////////
/**
This callback API is to call back when it receives a data_count that is set in FJ_UART_Receive.
*/
VOID FJ_Host_UART_Receive_CB( UCHAR ch, FJ_UART_CB_INFO cb_info )
{
	return;
}

/**
This callback API is to call back when it receives a slave_addr that is set in FJ_UART_Set_Slave_Addr.
*/
VOID FJ_Host_UART_recv_Slave_Addr_CB( UCHAR ch, FJ_UART_CB_INFO cb_info )
{
	return;
}

///////////////////////////////////////////////////////////////////////////////
// UPDATER
///////////////////////////////////////////////////////////////////////////////
/**
This callback function for update progress.
*/
VOID FJ_Updater_Progress_CB(E_FJ_UPDATER_PARTITION state, ULONG progress)
{
	printf("Partition data%d : %d%%\n", (UINT32)state, (UINT32)progress);
	return;
}


