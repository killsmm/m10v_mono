/**
 * @file		audio.c
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "ddim_typedef.h"
#include "fj_audio.h"
#include "fj_encoder.h"
#include "fj_decoder.h"
#include "audio.h"
#include "audio_source.h"
#include "audio_capture.h"
#include "audio_out.h"
#include "audio_buzzer.h"
#include "audio_encoder.h"
#include "audio_decoder.h"
#include "os_user_custom.h"
#include "instance.h"


/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
T_BF_INSTANCE_AUDIO_SRC gBF_Instance_AudioSRC[D_BF_INSTANCE_ID_MAX_AUDIO_SRC] =
{
	{
		E_BF_INSTANCE_FLAG_UNUSE,						// flg
		E_FJ_AUDIO_SRC_0,								// type
		E_BF_INSTANCE_STATE_NORMAL,						// state
	},
	{
		E_BF_INSTANCE_FLAG_UNUSE,						// flg
		E_FJ_AUDIO_SRC_1,								// type
		E_BF_INSTANCE_STATE_NORMAL,						// state
	},
	{
		E_BF_INSTANCE_FLAG_UNUSE,						// flg
		E_FJ_AUDIO_SRC_2,								// type
		E_BF_INSTANCE_STATE_NORMAL,						// state
	}
};

T_BF_INSTANCE_AUDIO_CAP gBF_Instance_AudioCap[D_BF_INSTANCE_ID_MAX_AUDIO_CAP] =
{
	{
		E_BF_INSTANCE_FLAG_UNUSE,						// flg
		E_FJ_AUDIO_CAPTURE_0,							// type
		E_BF_INSTANCE_STATE_NORMAL,						// state
		{												// connect
			{											// audio_encoder
				0xFF,									// instance_id
				NULL									// instance_p
			},
			{											// audio_out
				0xFF,									// instance_id
				NULL									// instance_p
			}
		}
	},
	{
		E_BF_INSTANCE_FLAG_UNUSE,						// flg
		E_FJ_AUDIO_CAPTURE_1,							// type
		E_BF_INSTANCE_STATE_NORMAL,						// state
		{												// connect
			{											// audio_encoder
				0xFF,									// instance_id
				NULL									// instance_p
			},
			{											// audio_out
				0xFF,									// instance_id
				NULL									// instance_p
			}
		}
	}
};

T_BF_INSTANCE_AUDIO_OUT gBF_Instance_AudioOut[D_BF_INSTANCE_ID_MAX_AUDIO_OUT] = 
{
	{
		E_BF_INSTANCE_FLAG_UNUSE,						// flg
		E_FJ_AUDIO_OUT_0,								// type
		E_BF_INSTANCE_STATE_NORMAL,						// state
	},
	{
		E_BF_INSTANCE_FLAG_UNUSE,						// flg
		E_FJ_AUDIO_OUT_1,								// type
		E_BF_INSTANCE_STATE_NORMAL,						// state
	}
};

T_BF_INSTANCE_AUDIO_ENC gBF_Instance_AudioEnc[D_BF_INSTANCE_ID_MAX_AUDIO_ENC] = 
{
	{
		E_BF_INSTANCE_FLAG_UNUSE,						// flg
		E_FJ_AUDIO_ENC_AAC_0,							// type
		E_BF_INSTANCE_STATE_NORMAL,						// state
	},
	{
		E_BF_INSTANCE_FLAG_UNUSE,						// flg
		E_FJ_AUDIO_ENC_AAC_1,							// type
		E_BF_INSTANCE_STATE_NORMAL,						// state
	}
};

T_BF_INSTANCE_AUDIO_DEC gBF_Instance_AudioDec[D_BF_INSTANCE_ID_MAX_AUDIO_DEC] =
{
	{
		E_BF_INSTANCE_FLAG_UNUSE,						// flg
		E_FJ_AUDIO_DEC_AAC,								// type
		E_BF_INSTANCE_STATE_NORMAL,						// state
		{												// connect
			{											// audio_out
				0xFF,									// instance_id
				NULL									// instance_p
			}
		}
	}
};

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function                                                             */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/


/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/

// AUDIO SRC
/**
 *  @brief		Open Audio Source Instance
 *	@param		E_FJ_AUDIO_SRC_ENUM	Audio Source
 *	@param		UCHAR*				Audio Source ID
 *	@return		FJ_ERR_CODE			Result(FJ_ERR_OK/FJ_ERR_NG/FJ_ERR_NG_INPUT_PARAM)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_src_open( E_FJ_AUDIO_SRC_ENUM audio_src, UCHAR* audio_src_id )
{
	INT32 ret;
	UINT32 loop;
	
	if (audio_src_id == NULL){
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	if (audio_src > E_FJ_AUDIO_SRC_2){
		*audio_src_id = 0;
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	for (loop = 0; loop < D_BF_INSTANCE_ID_MAX_AUDIO_SRC; loop++){
		if ((gBF_Instance_AudioSRC[loop].type == audio_src)					&&
			(gBF_Instance_AudioSRC[loop].flag == E_BF_INSTANCE_FLAG_USE)	){
			*audio_src_id = 0;
			return FJ_ERR_NG_INPUT_PARAM;
		}
	}
	
	ret = BF_AudioSrc_Open(audio_src, audio_src_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioSRC[*audio_src_id - 1].type = audio_src;
	gBF_Instance_AudioSRC[*audio_src_id - 1].flag = E_BF_INSTANCE_FLAG_USE;
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Close Audio Source Instance
 *	@param		UCHAR			Audio Source ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_src_close( UCHAR audio_src_id )
{
	INT32 ret;
	
	ret = BF_AudioSrc_Close(audio_src_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioSRC[audio_src_id - 1].flag = E_BF_INSTANCE_FLAG_UNUSE;
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Connect Audio Source Instance with Audio Capture Instance
 *	@param		UCHAR			Audio Source ID
 *	@param		UCHAR			Audio Capture ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_src_connect( UCHAR audio_src_id, UCHAR audio_capture_id )
{
	INT32 ret;
	
	ret = BF_AudioSrc_Connect(audio_src_id, audio_capture_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Disconnect Audio Capture Instance
 *	@param		UCHAR			Audio Source ID
 *	@param		UCHAR			Audio Capture ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_src_disconnect( UCHAR audio_src_id, UCHAR audio_capture_id )
{
	INT32 ret;
	
	ret = BF_AudioSrc_Disconnect(audio_src_id, audio_capture_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}


// AUDIO CAPTURE
/**
 *  @brief		Open Audio Capture Instance
 *	@param		E_FJ_AUDIO_CAPTURE_ENUM	Audio Capture
 *	@param		UCHAR*				Audio Capture ID
 *	@return		FJ_ERR_CODE			Result(FJ_ERR_OK/FJ_ERR_NG/FJ_ERR_NG_INPUT_PARAM)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_capture_open( E_FJ_AUDIO_CAPTURE_ENUM audio_cap,  UCHAR* audio_capture_id )
{
	INT32 ret;
	UINT32 loop;
	
	if (audio_capture_id == NULL){
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	if (audio_cap > E_FJ_AUDIO_CAPTURE_2){
		*audio_capture_id = 0;
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	for (loop = 0; loop < D_BF_INSTANCE_ID_MAX_AUDIO_CAP; loop++){
		if ((gBF_Instance_AudioCap[loop].type == audio_cap)					&&
			(gBF_Instance_AudioCap[loop].flag == E_BF_INSTANCE_FLAG_USE)	){
			*audio_capture_id = 0;
			return FJ_ERR_NG_INPUT_PARAM;
		}
	}
	
	ret = BF_AudioCapture_Open(audio_cap, audio_capture_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioCap[*audio_capture_id - 1].type = audio_cap;
	gBF_Instance_AudioCap[*audio_capture_id - 1].flag = E_BF_INSTANCE_FLAG_USE;
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Close Audio Capture Instance
 *	@param		UCHAR			Audio Capture ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_capture_close( UCHAR audio_capture_id )
{
	INT32 ret;
	
	ret = BF_AudioCapture_Close(audio_capture_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioCap[audio_capture_id - 1].flag = E_BF_INSTANCE_FLAG_UNUSE;
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Connect Audio Capture Instance with Audio Out Instance
 *	@param		UCHAR			Audio Capture ID
 *	@param		UCHAR			Audio Out ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_capture_connect_out( UCHAR audio_capture_id, UCHAR audio_out_id )
{
	INT32 ret;
	
	ret = BF_AudioCapture_ConnectOut(audio_capture_id, audio_out_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	gBF_Instance_AudioCap[audio_capture_id - 1].connect.audio_out.instance_id = audio_out_id;
	gBF_Instance_AudioCap[audio_capture_id - 1].connect.audio_out.instance_p = (UCHAR*)&gBF_Instance_AudioOut[audio_out_id - 1];
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Disconnect Audio Out Instance
 *	@param		UCHAR			Audio Capture ID
 *	@param		UCHAR			Audio Out ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_capture_disconnect_out( UCHAR audio_capture_id, UCHAR audio_out_id )
{
	INT32 ret;
	
	ret = BF_AudioCapture_DisconnectOut(audio_capture_id, audio_out_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioCap[audio_capture_id - 1].connect.audio_out.instance_id = 0xFF;
	gBF_Instance_AudioCap[audio_capture_id - 1].connect.audio_out.instance_p = (UCHAR*)NULL;
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Connect Audio Capture Instance with Audio Encoder Instance
 *	@param		UCHAR			Audio Capture ID
 *	@param		UCHAR			Audio Encoder ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_capture_connect_enc( UCHAR audio_capture_id, UCHAR audio_enc_id )
{
	INT32 ret;
	
	ret = BF_AudioCapture_ConnectEnc(audio_capture_id, audio_enc_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	gBF_Instance_AudioCap[audio_capture_id - 1].connect.audio_encoder.instance_id = audio_enc_id;
	gBF_Instance_AudioCap[audio_capture_id - 1].connect.audio_encoder.instance_p = (UCHAR*)&gBF_Instance_AudioEnc[audio_enc_id - 1];
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Disconnect Audio Encoder Instance
 *	@param		UCHAR			Audio Capture ID
 *	@param		UCHAR			Audio Encoder ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_capture_disconnect_enc( UCHAR audio_capture_id, UCHAR audio_enc_id )
{
	INT32 ret;
	
	ret = BF_AudioCapture_DisconnectEnc(audio_capture_id, audio_enc_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioCap[audio_capture_id - 1].connect.audio_encoder.instance_id = 0xFF;
	gBF_Instance_AudioCap[audio_capture_id - 1].connect.audio_encoder.instance_p = (UCHAR*)NULL;
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Configure Audio Capture Instance
 *	@param		UCHAR			Audio Capture ID
 *	@param		T_FJ_AUDIO_CAPTURE_CFG	Configuration
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_capture_config( UCHAR audio_capture_id, T_FJ_AUDIO_CAPTURE_CFG* audio_capture_config )
{
	INT32 ret;
	
	ret = BF_AudioCapture_Config(audio_capture_id, audio_capture_config);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Sample Configure Audio Capture Instance
 *	@param		UCHAR			Audio Capture ID
 *	@param		T_FJ_AUDIO_CAP_SMPL_CFG	Configuration of Samples.
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_capture_sample_config( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_SMPL_CFG* audio_cap_smpl_config)
{
	INT32 ret;
	
	ret = BF_AudioCapture_SampleConfig(audio_capture_id, audio_cap_smpl_config);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Set Input Volume
 *	@param		UCHAR			Audio Capture ID
 *	@param		UINT8			Volume
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_capture_set_volume( UCHAR audio_capture_id, UINT8 volume )
{
	INT32 ret;
	
	ret = BF_AudioCapture_Set_Volume(audio_capture_id, volume);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Start Capture
 *	@param		UCHAR			Audio Capture ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_capture_start( UCHAR audio_capture_id )
{
	INT32 ret;
	
	ret = BF_AudioCapture_Start(audio_capture_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Stop Capture
 *	@param		UCHAR			Audio Capture ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_capture_stop( UCHAR audio_capture_id )
{
	INT32 ret;
	
	ret = BF_AudioCapture_Stop(audio_capture_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Enable PlugIn1
 *	@param		UCHAR			Audio Capture ID
 *	@param		UCHAR			enable/disable
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_capture_enable_plugin1( UCHAR audio_capture_id, UCHAR enable )
{
	INT32 ret;
	
	ret = BF_AudioCapture_EnablePlugIn1(audio_capture_id, enable);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Notification of finishing PlugIn1 Process.
 *	@param		UCHAR			Audio Capture ID
 *	@param		UCHAR			ID of Area.
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_capture_complete_plugin1( UCHAR audio_capture_id, UCHAR free_area_index )
{
	INT32 ret;
	
	ret = BF_AudioCapture_CompletePlugIn1(audio_capture_id, free_area_index);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

// AUDIO OUT
/**
 *  @brief		Open Audio Out Instance
 *	@param		E_FJ_AUDIO_OUT_ENUM	Audio Out
 *	@param		UCHAR*				Audio Out ID
 *	@return		FJ_ERR_CODE			Result(FJ_ERR_OK/FJ_ERR_NG/FJ_ERR_NG_INPUT_PARAM)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_out_open( E_FJ_AUDIO_OUT_ENUM audio_out,  UCHAR* audio_out_id )
{
	INT32 ret;
	UINT32 loop;
	
	if (audio_out_id == NULL){
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	if (audio_out > E_FJ_AUDIO_OUT_1){
		*audio_out_id = 0;
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	for (loop = 0; loop < D_BF_INSTANCE_ID_MAX_AUDIO_OUT; loop++){
		if ((gBF_Instance_AudioOut[loop].type == audio_out)					&&
			(gBF_Instance_AudioOut[loop].flag == E_BF_INSTANCE_FLAG_USE)	){
			*audio_out_id = 0;
			return FJ_ERR_NG_INPUT_PARAM;
		}
	}
	
	ret = BF_AudioOut_Open(audio_out, audio_out_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioOut[*audio_out_id - 1].type = audio_out;
	gBF_Instance_AudioOut[*audio_out_id - 1].flag = E_BF_INSTANCE_FLAG_USE;
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Close Audio Out Instance
 *	@param		UCHAR			Audio Out ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_out_close( UCHAR audio_out_id )
{
	INT32 ret;
	
	ret = BF_AudioOut_Close(audio_out_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioOut[audio_out_id - 1].flag = E_BF_INSTANCE_FLAG_UNUSE;
	
	return FJ_ERR_OK;
}

FJ_ERR_CODE fj_audio_out_forceclose( UCHAR audio_out_id )
{
	INT32 ret;
	
	ret = BF_AudioOut_ForceClose(audio_out_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioOut[audio_out_id - 1].flag = E_BF_INSTANCE_FLAG_UNUSE;
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Configure Audio Out Instance
 *	@param		UCHAR			Audio Out ID
 *	@param		T_FJ_AUDIO_OUT_CFG	Configuration
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_out_config( UCHAR audio_out_id, T_FJ_AUDIO_OUT_CFG* audio_out_config )
{
	INT32 ret;
	OS_USER_ER ercd;
	
	ercd = OS_User_Wai_Sem(SID_AUDIO_COMMON);
	if (D_OS_USER_E_OK != ercd){
		return FJ_ERR_NG;
	}
	
	ret = BF_AudioOut_Config(audio_out_id, audio_out_config);
	
	OS_User_Sig_Sem(SID_AUDIO_COMMON);
	
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Set Output Volume
 *	@param		UCHAR			Audio Out ID
 *	@param		UINT8			Volume
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_out_set_volume( UCHAR audio_out_id, UINT8 volume )
{
	INT32 ret;
	
	ret = BF_AudioOut_Set_Volume(audio_out_id, volume);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}


/**
 *  @brief		Load Audio Data from wave format.
 *	@param		UINT32			address of stored wave format data
 *	@param		ULONG			size of stored wave format data
 *	@param		UINT32*			ID of loaded audio data
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_out_direct_load_wav( UINT32 addr, ULONG size, UINT32* wav_id )
{
	INT32 ret;
	OS_USER_ER ercd;
	
	if (wav_id == NULL){
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	ercd = OS_User_Wai_Sem(SID_AUDIO_COMMON);
	if (D_OS_USER_E_OK != ercd){
		return FJ_ERR_NG;
	}
	
	ret = BF_AudioBuzzer_DirectLoadWav(addr, size, wav_id);
	
	OS_User_Sig_Sem(SID_AUDIO_COMMON);
	
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Unload Audio Data.
 *	@param		UINT32			ID of loaded audio data
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_out_direct_unload_wav( UINT32 wav_id )
{
	INT32 ret;
	OS_USER_ER ercd;
	
	ercd = OS_User_Wai_Sem(SID_AUDIO_COMMON);
	if (D_OS_USER_E_OK != ercd){
		return FJ_ERR_NG;
	}
	
	ret = BF_AudioBuzzer_DirectUnloadWav(wav_id);
	
	OS_User_Sig_Sem(SID_AUDIO_COMMON);
	
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Play loaded Audio Data.
 *	@param		UCHAR			Audio Out Instance
 *	@param		UINT32			ID of loaded audio data
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_out_direct_play_wav( UCHAR audio_out_id, UINT32 wav_id )
{
	INT32 ret;
	OS_USER_ER ercd;
	
	ercd = OS_User_Wai_Sem(SID_AUDIO_COMMON);
	if (D_OS_USER_E_OK != ercd){
		return FJ_ERR_NG;
	}
	
	ret = BF_AudioBuzzer_DirectPlayWav(audio_out_id, wav_id);
	
	OS_User_Sig_Sem(SID_AUDIO_COMMON);
	
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Stop playback of loaded Audio Data.
 *	@param		UCHAR			Audio Out Instance
 *	@param		UINT32			ID of loaded audio data
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_out_direct_stop_wav( UCHAR audio_out_id, UINT32 wav_id )
{
	INT32 ret;
	OS_USER_ER ercd;
	
	ercd = OS_User_Wai_Sem(SID_AUDIO_COMMON);
	if (D_OS_USER_E_OK != ercd){
		return FJ_ERR_NG;
	}
	
	ret = BF_AudioBuzzer_DirectStopWav(audio_out_id, wav_id);
	
	OS_User_Sig_Sem(SID_AUDIO_COMMON);
	
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Control playback of Movie Sound and Beep Sound.
 *	@param		UCHAR			Audio Out Instance
 *	@param		E_FJ_AUDIO_WAV_CTL			Control Information
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_out_direct_control_wav( UCHAR audio_out_id, E_FJ_AUDIO_WAV_CTL control )
{
	INT32 ret;
	OS_USER_ER ercd;
	
	ercd = OS_User_Wai_Sem(SID_AUDIO_COMMON);
	if (D_OS_USER_E_OK != ercd){
		return FJ_ERR_NG;
	}
	
	ret = BF_AudioBuzzer_DirectControlWav(audio_out_id, control);
	
	OS_User_Sig_Sem(SID_AUDIO_COMMON);
	
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}


// AUDIO ENCODER
/**
 *  @brief		Open Audio Encoder Instance
 *	@param		E_FJ_AUDIO_ENC_ENUM	Audio Encoder
 *	@param		UCHAR*				Audio Encoder ID
 *	@return		FJ_ERR_CODE			Result(FJ_ERR_OK/FJ_ERR_NG/FJ_ERR_NG_INPUT_PARAM)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_encoder_open( E_FJ_AUDIO_ENC_ENUM audio_enc, UCHAR* audio_enc_id )
{
	INT32 ret;
	UINT32 loop;
	
	if (audio_enc_id == NULL){
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	if (audio_enc > E_FJ_AUDIO_ENC_AAC_1){
		*audio_enc_id = 0;
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	for (loop = 0; loop < D_BF_INSTANCE_ID_MAX_AUDIO_ENC; loop++){
		if ((gBF_Instance_AudioEnc[loop].type == audio_enc)					&&
			(gBF_Instance_AudioEnc[loop].flag == E_BF_INSTANCE_FLAG_USE)	){
			*audio_enc_id = 0;
			return FJ_ERR_NG_INPUT_PARAM;
		}
	}
	
	ret = BF_AudioEnc_Open(audio_enc, audio_enc_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioEnc[*audio_enc_id - 1].type = audio_enc;
	gBF_Instance_AudioEnc[*audio_enc_id - 1].flag = E_BF_INSTANCE_FLAG_USE;
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Close Audio Encoder Instance
 *	@param		UCHAR			Audio Encoder ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_encoder_close( UCHAR audio_enc_id )
{
	INT32 ret;
	
	ret = BF_AudioEnc_Close(audio_enc_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioEnc[audio_enc_id - 1].flag = E_BF_INSTANCE_FLAG_UNUSE;
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Free Buffer of Audio Encoder
 *	@param		UCHAR			Audio Encoder ID
 *	@param		UINT32			Stream Buffer ID
 *	@param		BOOL			Flag of Last Free Request
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_encoder_free_index( UCHAR audio_enc_id, UINT32 free_astream_index, BOOL last_free_flg )
{
	INT32 ret;
	
	ret = BF_AudioEnc_FreeIndex(audio_enc_id, free_astream_index, last_free_flg);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Configure Audio Encoder Instance
 *	@param		UCHAR			Audio Encoder ID
 *	@param		UINT32			Configuration Format
 *	@param		UINT32*			Configuration Information
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_encoder_config( UCHAR audio_enc_id, UINT32 wConfig, UINT32* wOperation )
{
	INT32 ret;
	
	ret = BF_AudioEnc_Config(audio_enc_id, (T_FJ_AUDIO_AAC_CONFIG_PARAM*)wOperation);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Setup Audio Encoder
 *	@param		UCHAR			Audio Encoder ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_encoder_setup( UCHAR audio_enc_id )
{
	INT32 ret;
	
	ret = BF_AudioEnc_Setup(audio_enc_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Start Audio Encoder
 *	@param		UCHAR			Audio Encoder ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_encoder_start( UCHAR audio_enc_id )
{
	INT32 ret;
	
	ret = BF_AudioEnc_Start(audio_enc_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_TIMEOUT){
			return FJ_ERR_NG_TMOUT;
		}
		else if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Stop Audio Encoder
 *	@param		UCHAR			Audio Encoder ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_encoder_stop( UCHAR audio_enc_id )
{
	INT32 ret;
	
	ret = BF_AudioEnc_Stop(audio_enc_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Start Audio Encoder
 *	@param		UCHAR			Audio Encoder ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_encoder_startmute( UCHAR audio_enc_id )
{
	INT32 ret;
	
	ret = BF_AudioEnc_StartMute(audio_enc_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Stop Audio Encoder
 *	@param		UCHAR			Audio Encoder ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_encoder_stopmute( UCHAR audio_enc_id )
{
	INT32 ret;
	
	ret = BF_AudioEnc_StopMute(audio_enc_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

// AUDIO DECODER
/**
 *  @brief		Open Audio Decoder Instance
 *	@param		E_FJ_AUDIO_DEC_ENUM	Audio Decoder
 *	@param		UCHAR*				Audio Decoder ID
 *	@return		FJ_ERR_CODE			Result(FJ_ERR_OK/FJ_ERR_NG/FJ_ERR_NG_INPUT_PARAM)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_decoder_open( E_FJ_AUDIO_DEC_ENUM audio_dec,  UCHAR* audio_dec_id )
{
	INT32 ret;
	UINT32 loop;
	
	if (audio_dec_id == NULL){
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	if (audio_dec > E_FJ_AUDIO_DEC_AAC){
		*audio_dec_id = 0;
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	for (loop = 0; loop < D_BF_INSTANCE_ID_MAX_AUDIO_DEC; loop++){
		if ((gBF_Instance_AudioDec[loop].type == audio_dec)					&&
			(gBF_Instance_AudioDec[loop].flag == E_BF_INSTANCE_FLAG_USE)	){
			*audio_dec_id = 0;
			return FJ_ERR_NG_INPUT_PARAM;
		}
	}
	
	ret = BF_AudioDec_Open(audio_dec, audio_dec_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioDec[*audio_dec_id - 1].type = audio_dec;
	gBF_Instance_AudioDec[*audio_dec_id - 1].flag = E_BF_INSTANCE_FLAG_USE;
	

	return FJ_ERR_OK;
}

/**
 *  @brief		Close Audio Decoder Instance
 *	@param		UCHAR			Audio Decoder ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_decoder_close( UCHAR audio_dec_id )
{
	INT32 ret;
	
	ret = BF_AudioDec_Close(audio_dec_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioDec[audio_dec_id - 1].flag = E_BF_INSTANCE_FLAG_UNUSE;
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Connect Audio Decoder Instance with Audio Out Instance
 *	@param		UCHAR			Audio Decoder ID
 *	@param		UCHAR			Audio Out ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_decoder_connect( UCHAR audio_dec_id, UCHAR audio_out_id )
{
	INT32 ret;
	
	ret = BF_AudioDec_Connect(audio_dec_id, audio_out_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	gBF_Instance_AudioDec[audio_dec_id - 1].connect.audio_out.instance_id = audio_out_id;
	gBF_Instance_AudioDec[audio_dec_id - 1].connect.audio_out.instance_p = (UCHAR*)&gBF_Instance_AudioOut[audio_out_id - 1];
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Disconnect Audio Out Instance
 *	@param		UCHAR			Audio Decoder ID
 *	@param		UCHAR			Audio Out ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_decoder_disconnect( UCHAR audio_dec_id, UCHAR audio_out_id )
{
	INT32 ret;
	
	ret = BF_AudioDec_Disconnect(audio_dec_id, audio_out_id);
	if (ret != D_AUDIO_OK){
		return FJ_ERR_NG;
	}
	
	gBF_Instance_AudioDec[audio_dec_id - 1].connect.audio_out.instance_id = 0xFF;
	gBF_Instance_AudioDec[audio_dec_id - 1].connect.audio_out.instance_p = (UCHAR*)NULL;
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Initialize Audio Decoder
 *	@param		UCHAR			Audio Decoder ID
 *	@param		UCHAR			Audio Out ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_decoder_init( UCHAR audio_dec_id )
{
	INT32 ret;
	
	ret = BF_AudioDec_Init(audio_dec_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Configure Audio Decoder Instance
 *	@param		UCHAR			Audio Decoder ID
 *	@param		T_FJ_AUDIO_DEC_CONFIG_PARAM	Configuration
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_decoder_config( UCHAR audio_dec_id, T_FJ_AUDIO_DEC_CONFIG_PARAM* audio_dec_config )
{
	INT32 ret;
	
	ret = BF_AudioDec_Config(audio_dec_id, audio_dec_config);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Set Audio Stream for decode
 *	@param		UCHAR			Audio Decoder ID
 *	@param		UINT64			PTS
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_decoder_set_audio_stream( UCHAR audio_dec_id, UINT32 store_num, T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM* audio_stream )
{
	INT32 ret;
	
	ret = BF_AudioDec_SetAudioStream(audio_dec_id, store_num, audio_stream);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else if (ret == D_AUDIO_ERR_OVERFLOW){
			return FJ_ERR_OVERFLOW;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Start Audio Decoder
 *	@param		UCHAR			Audio Decoder ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_decoder_start_decode( UCHAR audio_dec_id )
{
	INT32 ret;
	
	ret = BF_AudioDec_Start(audio_dec_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Stop Audio Decoder
 *	@param		UCHAR			Audio Decoder ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_decoder_stop_decode( UCHAR audio_dec_id )
{
	INT32 ret;
	
	ret = BF_AudioDec_Stop(audio_dec_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Start Output Decoded Audio Data
 *	@param		UCHAR			Audio Decoder ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_decoder_start_output( UCHAR audio_dec_id )
{
	INT32 ret;
	
	ret = BF_AudioDec_StartOutput(audio_dec_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Stop Output Decoded Audio Data
 *	@param		UCHAR			Audio Decoder ID
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_decoder_stop_output( UCHAR audio_dec_id )
{
	INT32 ret;
	
	ret = BF_AudioDec_StopOutput(audio_dec_id);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}

/**
 *  @brief		Allocate Memory
 *	@param		UCHAR			Audio Decoder ID
 *	@param		UINT32*			Size of memory
 *	@param		UINT32*			Address of memory
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_audio_decoder_mem_allocate( UCHAR audio_dec_id, UINT32* size, UINT32* address )
{
	INT32 ret;
	
	if (size == NULL){
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	if (address == NULL){
		return FJ_ERR_NG_INPUT_PARAM;
	}
	
	ret = BF_AudioDec_MemAllocate(audio_dec_id, size, address);
	if (ret != D_AUDIO_OK){
		if (ret == D_AUDIO_ERR_PARAM){
			return FJ_ERR_NG_INPUT_PARAM;
		}
		else {
			return FJ_ERR_NG;
		}
	}
	
	return FJ_ERR_OK;
}


// For VideoPlayback
/**
 *	@brief		Initialize Audio Decoder for Video Playback
 *	@param		None
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_videoplayback_audioinit( VOID )
{
	return fj_audio_decoder_init(FJ_AUDIO_DEC_ID_0);
}

/**
 *	@brief		Set Audio Stream for Video Playback
 *	@param		None
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_videoplayback_setaudiostream( UINT32 store_num, T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM* audio_stream_param )
{
	return fj_audio_decoder_set_audio_stream(FJ_AUDIO_DEC_ID_0, store_num, audio_stream_param);
}

/**
 *	@brief		Start Audio Decoder for Video Playback
 *	@param		None
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_videoplayback_startaudiodecode( VOID )
{
	return fj_audio_decoder_start_decode(FJ_AUDIO_DEC_ID_0);
}

/**
 *	@brief		Start output decoded audio data for Video Playback
 *	@param		None
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_videoplayback_startaudiooutput( VOID )
{
	return fj_audio_decoder_start_output(FJ_AUDIO_DEC_ID_0);
}

/**
 *	@brief		Stop output decoded audio data for Video Playback
 *	@param		None
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_videoplayback_stopaudiooutput( VOID )
{
	return fj_audio_decoder_stop_output(FJ_AUDIO_DEC_ID_0);
}

/**
 *	@brief		Stop Audio decoder for Video Playback
 *	@param		None
 *	@return		FJ_ERR_CODE		Result(FJ_ERR_OK/FJ_ERR_NG)
 *	@note		None
 *	@attention	None
 */
FJ_ERR_CODE fj_videoplayback_stopaudiodecode( VOID )
{
	return fj_audio_decoder_stop_decode(FJ_AUDIO_DEC_ID_0);
}

/********************************************************************************/
/*		Get Instance API														*/
/********************************************************************************/

/**
 *	@brief		Get AudioSRC instance.
 *	@param		UCHAR						Audio Source ID
 *	@return		T_BF_INSTANCE_AUDIO_SRC*	Address of Audio Source Instance
 *	@note		None
 *	@attention	None
 */
T_BF_INSTANCE_AUDIO_SRC* BF_Instance_Get_AudioSrc( UCHAR audio_src_id )
{
	return (T_BF_INSTANCE_AUDIO_SRC*)&gBF_Instance_AudioSRC[audio_src_id];
}

/**
 *	@brief		Get AudioCapture instance.
 *	@param		UCHAR						Audio Capture ID
 *	@return		T_BF_INSTANCE_AUDIO_CAP*	Address of Audio Capture Instance
 *	@note		None
 *	@attention	None
 */
T_BF_INSTANCE_AUDIO_CAP* BF_Instance_Get_AudioCapture( UCHAR audio_capture_id )
{
	return (T_BF_INSTANCE_AUDIO_CAP*)&gBF_Instance_AudioCap[audio_capture_id];
}

/**
 *	@brief		Get AudioOut instance.
 *	@param		UCHAR						Audio Out ID
 *	@return		T_BF_INSTANCE_AUDIO_OUT*	Address of Audio Out Instance
 *	@note		None
 *	@attention	None
 */
T_BF_INSTANCE_AUDIO_OUT* BF_Instance_Get_AudioOut( UCHAR audio_out_id )
{
	return (T_BF_INSTANCE_AUDIO_OUT*)&gBF_Instance_AudioOut[audio_out_id];
}

/**
 *	@brief		Get AudioEncoder instance.
 *	@param		UCHAR						Audio Encoder ID
 *	@return		T_BF_INSTANCE_AUDIO_ENC*	Address of Audio Encoder Instance
 *	@note		None
 *	@attention	None
 */
T_BF_INSTANCE_AUDIO_ENC* BF_Instance_Get_AudioEncoder( UCHAR audio_enc_id )
{
	return (T_BF_INSTANCE_AUDIO_ENC*)&gBF_Instance_AudioEnc[audio_enc_id];
}

/**
 *	@brief		Get AudioDecoder instance.
 *	@param		UCHAR						Audio Decoder ID
 *	@return		T_BF_INSTANCE_AUDIO_DEC*	Address of Audio Decoder Instance
 *	@note		None
 *	@attention	None
 */
T_BF_INSTANCE_AUDIO_DEC* BF_Instance_Get_AudioDecoder( UCHAR audio_dec_id )
{
	return (T_BF_INSTANCE_AUDIO_DEC*)&gBF_Instance_AudioDec[audio_dec_id];
}

