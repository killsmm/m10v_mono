/**
 * @file		audio_aac_enc.c
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include <stdio.h>
#include "ddim_typedef.h"
#include "audio_aac_enc.h"
//### --- REMOVE_AAC BEGIN ---
#include "aacencoder.h"
//### --- REMOVE_AAC END ---
#include "audio_aac_if.h"
#include "audio_data.h"
#include "debug.h"


/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/
//### --- REMOVE_AAC BEGIN ---
static volatile UINT16 gBF_AudioAAC_Enc_ChannelStatus[D_AUDIO_AAC_ENC_NUM];
static volatile UINT32 gBF_AudioAAC_Enc_Endian[D_AUDIO_AAC_ENC_NUM];
static volatile UINT32 gBF_AudioAAC_Enc_Format[D_AUDIO_AAC_ENC_NUM];
static volatile AACE_MemorySize gBF_AudioAAC_Enc_MemSize[D_AUDIO_AAC_ENC_NUM];
static volatile AACE_MemoryAdrs gBF_AudioAAC_Enc_MemAdrs[D_AUDIO_AAC_ENC_NUM];
static volatile ULONG gBF_AudioAAC_Enc_Context[D_AUDIO_AAC_ENC_NUM][0x24DF] __attribute__((aligned(8)));
//### --- REMOVE_AAC END ---

/*
/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
                                                                                
   AAC ENCODE INITIALIZE                                                        
                                                                                
/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
*/
UINT32 BF_AudioAAC_Enc_Init( T_AUDIO_AAC_ENC_INIT* param )
{
	UINT32 ret_func = AAC_RET_SUCCESS;
//### --- REMOVE_AAC BEGIN ---
	AACE_retMemSize retMemsize;
	AACE_ProcessMode select_mode;
	ULONG audioSpecificConfigBitSize;
	AACE_SamplingRate sampling_rate;
	UINT32 bitrate;
	UINT32 ret = 0;
	UINT8 audio_specific_config[9] = {0};
	UINT8* spec_addr;
	const AACE_SamplingRate sampling_table[9] = {AACE_SAMPLING_RATE_8000Hz,		AACE_SAMPLING_RATE_11025Hz,	AACE_SAMPLING_RATE_12000Hz,
												 AACE_SAMPLING_RATE_16000Hz,	AACE_SAMPLING_RATE_22050Hz,	AACE_SAMPLING_RATE_24000Hz,
												 AACE_SAMPLING_RATE_32000Hz,	AACE_SAMPLING_RATE_44100Hz,	AACE_SAMPLING_RATE_48000Hz};
	const USHORT channel_table[4]  = {AACE_STEREO_MODE, 0, AACE_BILINGUAL_MODE, AACE_MONAURAL_MODE};
	UINT8 aac_id;
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AAC_ENC]BF_AudioAAC_Enc_Init() param err. L=%d\n", __LINE__));
		return AAC_RET_ERROR;
	}
	
	aac_id = param->aac_id;
	
	gBF_AudioAAC_Enc_Format[aac_id] = param->format;
	
	select_mode = AACE_NORMAL_MODE;
	spec_addr = audio_specific_config;
	
	do{
		// Getting the size information of context memory
		retMemsize = AACEncGetContextSize(&select_mode, (AACE_MemorySize *)&gBF_AudioAAC_Enc_MemSize[aac_id]);
		if ( retMemsize!= AACE_GMS_SUCCESS ){
			ret_func = AAC_RET_ERROR;
			break;
		}
		
		gBF_AudioAAC_Enc_MemAdrs[aac_id].ContextMem = (ULONG*)&gBF_AudioAAC_Enc_Context[aac_id][0];
		gBF_AudioAAC_Enc_MemAdrs[aac_id].IntMem1 = NULL;
		gBF_AudioAAC_Enc_MemAdrs[aac_id].IntMem2 = NULL;
		
		// Get Sampling Rate
		sampling_rate  = sampling_table[param->smpl_rate];
		
		// Get Channel Status
		gBF_AudioAAC_Enc_ChannelStatus[aac_id] = channel_table[param->channel];
		
		// Get Endian
		gBF_AudioAAC_Enc_Endian[aac_id] = param->endian;
		
		if (gBF_AudioAAC_Enc_ChannelStatus[aac_id] == 0){
			ret_func = AAC_RET_ERROR;
			break;
		}
		
		bitrate = param->bit_rate;
		
		if ((bitrate < 8000) || (256000 < bitrate)){
			ret_func = AAC_RET_ERROR;
			break;
		}
		
		// Setting encoding parameters
		ret = AACEncSetConfig( &select_mode,
							   (AACE_MemoryAdrs *)&gBF_AudioAAC_Enc_MemAdrs[aac_id],
							   (AACE_MemorySize *)&gBF_AudioAAC_Enc_MemSize[aac_id],
							   sampling_rate,
							   (AACE_ChannelStatus)gBF_AudioAAC_Enc_ChannelStatus[aac_id],
							   bitrate,
							   0,
							   0 );
		
		if (ret != 0){
			
			// Error:Setting encoding parameters
			ret_func = AAC_RET_ERROR;
			BF_Debug_Print_Error(("[AAC_ENC_Init]Set Encoding Parameter. Err=%d\n", ret));
			break;
		}
		
		if (gBF_AudioAAC_Enc_Format[aac_id] == AAC_LIB_AACENC_RAW){
			// AAC RAW
			
			audioSpecificConfigBitSize =16;
			
			// Getting Information on Encoding Parameters
			ret = AACEncGetAudioSpecificConfig((ULONG*)&gBF_AudioAAC_Enc_Context[aac_id][0], &audioSpecificConfigBitSize, &spec_addr);
			
			if (ret != 0x00){
				// Error: Getting Information on Encoding Parameters
				ret_func = AAC_RET_ERROR;
				BF_Debug_Print_Error(("[AAC_ENC_Init]Get Encoding Parameter. Err=%d\n", ret));
				break;
			}
			/* audio specific config is
				audioObjectType 					 5bit
				samplingFrequencyIndex				 4bit
				channelConfigration					 4bit
				GASpec...()	frameLengthFlag 		 1bit
							dependsOnCoreCoder		 1bit
							extensionFlag			 1bit
							programConfigElement()	56bit
				------------------------------------------
													72bit = 9byte
				
				Note: In case of Dual monoral setting, programConfigElement() is enable.
					  Because output size is 2byte exclusive of programConfigElement().
			*/
		}
	}
 	while (0);
	
	if ((gBF_AudioAAC_Enc_Format[aac_id] == AAC_LIB_AACENC_RAW) && (ret_func == AAC_RET_SUCCESS)){
		
		param->audio_spec_cfg = (audio_specific_config[0] * 0x100) + audio_specific_config[1];
	}
	else {
		param->audio_spec_cfg = 0;
	}
	
//### --- REMOVE_AAC END ---
	return ret_func;
}

/*
/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
                                                                                
   AAC ENCODE PROCESS                                                           
                                                                                
/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
*/	

#define LIMIT_MIN(A, B)	(((A) > (B)) ? (A) : (B))				// < A = A >=  B
#define LIMIT_MAX(A, B)	(((A) < (B)) ? (A) : (B))				// < A = A <=  B
#define LIMIT(A, B, C)	(LIMIT_MAX(LIMIT_MIN((A), (B)), (C)))	// < A = B <= A <= C

UINT32 BF_AudioAAC_Enc_Process( T_AUDIO_AAC_ENC_PROC* param )
{
	UINT32 ret_func = AAC_RET_SUCCESS;
//### --- REMOVE_AAC BEGIN ---
	AACE_InputDataHolder input_addr;
	SHORT separate_stream[2][1024];
	BYTE* inbuf;
	BYTE* outbuf;
	ULONG original_bytes;
	ULONG total_in_bytes, total_out_bytes;
	SHORT* data16;
	BYTE* data08;
	ULONG out_bytes;
	ULONG offset;
	ULONG ret;
	ULONG one_frame_bytes;
	USHORT turn, turn_max, turn_max_pos;
	SHORT buf;
	/*					+0,	 +1.5,	+3, +4.5,  +6, +7.5,  +9, +10.5, +12, +13.5, +15, +16.5,  +18 [dB]*/
	LONG gain_tbl[13] = {128, 152, 181,  215, 255,  304, 361,   429, 510,   606, 720,   855, 1017};
	USHORT soft_gain = 10;
	INT32 loop;
	UINT8 aac_id;
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AAC_ENC]BF_AudioAAC_Enc_Process() param err. L=%d\n", __LINE__));
		return AAC_RET_ERROR;
	}
	
	aac_id = param->aac_id;
	
	soft_gain = param->soft_gain;
	turn_max = 0;
	inbuf = (BYTE*)param->in_data_addr;
	original_bytes = (ULONG)param->in_data_size;
	outbuf = (BYTE *)param->out_data_addr;
	total_in_bytes = 0;
	total_out_bytes   = 0;
	turn = 0;
	
	
	if (gBF_AudioAAC_Enc_ChannelStatus[aac_id] != AACE_MONAURAL_MODE){
		// 2ch
		one_frame_bytes   = 2048 * 2;
		input_addr.center = NULL;
		input_addr.left   = (SHORT*)&separate_stream[0][0];
		input_addr.right  = (SHORT*)&separate_stream[1][0];
	}else{
		// 1ch
		one_frame_bytes   = 1024 * 2;
		input_addr.center = (SHORT*)&separate_stream[0][0];
		input_addr.left   = NULL;
		input_addr.right  = NULL;
	}
	
	/* Calculate the processing number of times. */
	turn_max_pos = original_bytes / one_frame_bytes;
	
	if ((turn_max == 0) || (turn_max > turn_max_pos)){
		turn_max = turn_max_pos;
	}
	
	/* Encoding processing loop */
	while (total_in_bytes <= original_bytes){
		
		if (gBF_AudioAAC_Enc_ChannelStatus[aac_id] == AACE_STEREO_MODE){
			/* 2ch (stereo, dual-mono) Input data handling before the encoding */
			data16 = (SHORT*)(inbuf + total_in_bytes);
			for (loop = 0; loop < 1024; loop++){
				// left
				buf = *data16;
				buf = LIMIT((((INT32)buf * gain_tbl[soft_gain]) >> 7), -32768, 32767);
				separate_stream[0][loop] = buf;
				data16++;
				
				// right
				buf = *data16;
				buf = LIMIT((((INT32)buf * gain_tbl[soft_gain]) >> 7), -32768, 32767);
				separate_stream[1][loop] = buf;
				data16++;
			}
			/* Endian conversion */
			// AAC dec. default endian is "big"
			if (gBF_AudioAAC_Enc_Endian[aac_id] == AAC_ENDIAN_BIG){
				BF_Audio_Data_ChangeEndian((BYTE*)&separate_stream[0][0], 2048);
				BF_Audio_Data_ChangeEndian((BYTE*)&separate_stream[1][0], 2048);
			}
		}
		else{
			/* 1ch (mono) Input data handling before the encoding */
			data16 = (SHORT*)(inbuf + total_in_bytes);
			for (loop = 0; loop < 1024; loop++){
				buf = *data16;
				buf = LIMIT((((INT32)buf * gain_tbl[soft_gain]) >> 7), -32768, 32767);
				separate_stream[0][loop] = buf;
				data16++;
			}

			/* Endian conversion */
			// AAC dec. default endian is "big"
			if (gBF_AudioAAC_Enc_Endian[aac_id] == AAC_ENDIAN_BIG){
				BF_Audio_Data_ChangeEndian((BYTE*)&separate_stream[0][0], 2048);
			}
		}
		// Encoding and storing encoded data
		ret = AACEncEncode((ULONG *)&gBF_AudioAAC_Enc_Context[aac_id][0], &input_addr, &out_bytes, &data08);
		if (ret == 0x0d){
			/* Warnig (There is no problem in the encoding data.) */
			/* It occurs, when a set point is not so good. */
			//ret_func = RET_WARNING;
		}
		else if(ret != 0x00){
			/* Encode Error */
			ret_func = AAC_RET_ERROR;
			BF_Debug_Print_Error(("[AAC_ENC]Encoding. Err=%lu\n", ret));
			break;
		}
		else{
			;
		}
		
		offset = 0;
		if (gBF_AudioAAC_Enc_Format[aac_id] == AAC_LIB_AACENC_RAW){
			/* Revise it to skip a header in the case of RAW, and to record it */
			ret = AACEncGetRawDataBlock((ULONG *)&gBF_AudioAAC_Enc_Context[aac_id][0], &offset);
			offset = out_bytes - offset;
		}
		/* AAC stream is copied to a destination address */
		for (loop = (out_bytes - offset - 1); loop >= 0; loop--){
			outbuf[total_out_bytes + loop] = data08[offset + loop];
		}
		
		total_in_bytes += one_frame_bytes;
		total_out_bytes  += (out_bytes - offset);
		turn ++;
		/* It is finished, when I reach the upper limit of the encoding number of times. */
		if ((turn_max != 0) && (turn >= turn_max)){
			break;
		}
	}
	while (0);
	
	param->out_data_size = total_out_bytes;
	
//### --- REMOVE_AAC END ---
	return ret_func;
}


UINT32 BF_AudioAAC_Enc_GetVersion( VOID )
{
	UINT32 version = 0;
	
//### --- REMOVE_AAC BEGIN ---
	version = AACEncGetVersion();
//### --- REMOVE_AAC END ---
	
	return version;
}

