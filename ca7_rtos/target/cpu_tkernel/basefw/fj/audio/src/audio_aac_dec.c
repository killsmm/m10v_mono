/**
 * @file		audio_aac_dec.c
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdio.h>
#include <string.h>
#include "ddim_typedef.h"
#include "audio_aac_dec.h"
//### --- REMOVE_AAC BEGIN ---
#include "aacdecoder.h"
//### --- REMOVE_AAC END ---
#include "audio_aac_if.h"
#include "audio_data.h"
#include "debug.h"

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

#define LIMIT_MIN(A, B)	(((A) > (B)) ? (A) : (B))				// < A = A >=  B
#define LIMIT_MAX(A, B)	(((A) < (B)) ? (A) : (B))				// < A = A <=  B
#define LIMIT(A, B, C)	(LIMIT_MAX(LIMIT_MIN((A), (B)), (C)))	// < A = B <= A <= C

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/

//### --- REMOVE_AAC BEGIN ---
static volatile ULONG * gBF_AudioAAC_Dec_Context = NULL;
static volatile ULONG gBF_AudioAAC_Dec_Endian;
static volatile ULONG g_audio_aac_dec_context[0x20B2] __attribute__((aligned(8)));
//### --- REMOVE_AAC END ---

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/
/*
/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
                                                                                
   AAC DECODE INITIALIZE                                                        
                                                                                
/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
*/
UINT32 BF_AudioAAC_Dec_Init( const T_AUDIO_AAC_DEC_INIT* param)
{
	UINT32 ret_func = AAC_RET_SUCCESS;
//### --- REMOVE_AAC BEGIN ---
	size_t contextBytes = 0;
	AACD_AudioSpecificConfig ret_spec_config;
	BYTE config[2];
	ULONG config_bytes;
	UINT32 audio_spec_cfg;
	
	do{
		/* Get Context */
		contextBytes = AACD_getContextSize();
		if (contextBytes > 33480){
			BF_Debug_Print_Error(("[AAC_DEC_Init]contextBytes Error: Byte=%d\n", contextBytes));
		}
		
		gBF_AudioAAC_Dec_Context = (ULONG *)g_audio_aac_dec_context;
		
		/* Initialize context(library) */
		AACD_initialize((ULONG *)gBF_AudioAAC_Dec_Context);
		
		/* The stream having channels more than 2ch is effective by down mixture */
//		AACD_setDownMix((ULONG *)gBF_AudioAAC_Dec_Context, AACD_TRUE);								// Not Support Function
		/* 1sample/1ch:2byte=16bit */
		AACD_setBytesPerSample((ULONG *)gBF_AudioAAC_Dec_Context, AACD_BYTES_PER_SAMPLE_2);
		
		if (param->format == AAC_LIB_AACDEC_RAW){
			/* *** Setting only in case of RAW data *** */
			/* Set AudioSpecificConfig */
			audio_spec_cfg = param->audio_spec_cfg;
			
			config[0]  = (BYTE)((audio_spec_cfg & 0x0000FF00) >> 8);
			config[1]  = (BYTE)(audio_spec_cfg & 0x000000FF);
			
			config_bytes = param->cfg_size;
			
			ret_spec_config = AACD_setAudioSpecificConfig((ULONG *)gBF_AudioAAC_Dec_Context, config, config_bytes);
			
			if (ret_spec_config != AACD_ASC_SUCCESS){
				/* Error: Set AudioSpecificConfig */
				ret_func = AAC_RET_ERROR;
				BF_Debug_Print_Error(("[AAC_DEC_Init]Set AudioSpecificConfig. Err=%d\n", ret_spec_config));
				
				break;
			}
		}
		
		gBF_AudioAAC_Dec_Endian = param->endian;
		
	}
	while (0);
	
//### --- REMOVE_AAC END ---
	return ret_func;
}

/*
/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
                                                                                
   AAC DECODE PROCESS                                                           
                                                                                
/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_
*/	

UINT32 BF_AudioAAC_Dec_Process(T_AUDIO_AAC_DEC_PROC* param)
{
	UINT32 ret_func = AAC_RET_SUCCESS;
//### --- REMOVE_AAC BEGIN ---
	const ULONG sampling[16] = {	AAC_SMP_96000, AAC_SMP_88200, AAC_SMP_64000, AAC_SMP_48000, 
									AAC_SMP_44100, AAC_SMP_32000, AAC_SMP_24000, AAC_SMP_22050, 
									AAC_SMP_16000, AAC_SMP_12000, AAC_SMP_11025, AAC_SMP_8000, 
									AAC_SMP_7350, 0x0F, 0x0F, 0x0F};
	
	AACD_OutputDataHolder decoded_addr;	
	BYTE* inbuf;
	BYTE* outbuf;
	ULONG original_bytes, total_in_bytes, total_out_bytes;
	ULONG in_bytes, out_bytes;
	BYTE channel_status = 0;
	USHORT turn;
	SHORT separate_stream[2][1048];
	SHORT* position;
	ULONG cnt;
	ULONG bitrate;
	USHORT soft_gain;
	ULONG ret;
	USHORT decode_end = 0;
	SHORT buf;
	/*					+0,	 +1.5,	+3, +4.5,  +6, +7.5,  +9, +10.5, +12, +13.5, +15, +16.5,  +18 [dB]*/
	LONG gain_tbl[13] = {128, 152, 181,  215, 255,  304, 361,   429, 510,   606, 720,   855, 1017};
	
	if (param == NULL){
		BF_Debug_Print_Error(("[AAC_DEC]BF_AudioAAC_Dec_Process() param err. L=%d\n", __LINE__));
		return AAC_RET_ERROR;
	}
	
	soft_gain = param->soft_gain;
	inbuf = (BYTE*)param->in_data_addr;
	original_bytes = param->in_data_size;
	outbuf = (BYTE*)param->out_data_addr;
	
	/* Address setting of the output data */
	decoded_addr.center			= (ULONG*)&separate_stream[0][0];
	decoded_addr.left			= (ULONG*)&separate_stream[0][0];
	decoded_addr.right			= (ULONG*)&separate_stream[1][0];
	decoded_addr.leftBack		= NULL;
	decoded_addr.rightBack		= NULL;
	decoded_addr.lowFrequencyEnhancement = NULL;
	
	total_in_bytes = 0;
	total_out_bytes = 0;
	turn = 0;
	
	while (1){
		in_bytes = original_bytes - total_in_bytes;
		if (in_bytes < 3){
			// It is finished, Because it does not reach 1 frame.
			break;
		}
		in_bytes += 3;
		
		/* AAC Decode Process */
		ret = AACD_decode((ULONG *)gBF_AudioAAC_Dec_Context, (BYTE*)((UINT32)inbuf + total_in_bytes), &in_bytes, &out_bytes, &decoded_addr);
		
		if (ret == 0){
			
		}
		else if ((ret & 0xF0000000) == 0xF0000000){
			/* Decode Error */
			ret_func = AAC_RET_ERROR;
			BF_Debug_Print_Error(("[AAC_DEC]aac decode. Err = %lx\n", ret));
			
			memset((VOID*)outbuf, 0, (1024 * 2 * 2));
			total_out_bytes = 1024 * 2 * 2;
			break;
		}
		else{
			// finish decode (success and warning decode)
			decode_end = 1;
			//break;
		}
		/* Get the Channel Information */
		if (turn == 0){

			if ( AACD_getSamplingRate((ULONG *)gBF_AudioAAC_Dec_Context) == AACD_SAMPLING_RATE_OTHERS ){
				param->out_sampling_rate = AAC_SMP_OTHER;
			}
			else {
				param->out_sampling_rate = sampling[AACD_getSamplingRate((ULONG *)gBF_AudioAAC_Dec_Context)];
			}
			
			if (AACD_getBitRate((ULONG *)gBF_AudioAAC_Dec_Context) < 0){
				bitrate = 0xFFFFFFFF;
			}
			else {
				bitrate = (ULONG)AACD_getBitRate((ULONG *)gBF_AudioAAC_Dec_Context);
			}
			
			param->out_bit_rate = bitrate;
			
			channel_status = AACD_getChannelStatus((ULONG *)gBF_AudioAAC_Dec_Context);
			param->out_channel = channel_status;
		}
		/* The data which I decoded are transferred to user memory area */
		if (AACD_CHANNEL_CENTER_ON == (channel_status & (AACD_CHANNEL_CENTER_ON | AACD_CHANNEL_LEFT_ON | AACD_CHANNEL_RIGHT_ON))){
			/* In case of monaural */
			position = (SHORT*)((UINT32)outbuf + total_out_bytes);
			
			for (cnt = 0; cnt < out_bytes; cnt ++){
				buf  = separate_stream[0][cnt];
				buf = LIMIT( (((LONG)buf * gain_tbl[soft_gain]) >> 7), -32768, 32767);
				*position = buf;
				position++;
			}
		}
		else{
			/* n case of stereo */
			position = (SHORT*)((UINT32)outbuf + total_out_bytes);
			for (cnt = 0; cnt < out_bytes; cnt ++){
				buf  = separate_stream[0][cnt];
				buf = LIMIT( (((LONG)buf * gain_tbl[soft_gain]) >> 7), -32768, 32767);
				*position = buf;
				position++;
				buf  = separate_stream[1][cnt];
				buf = LIMIT( (((LONG)buf * gain_tbl[soft_gain]) >> 7), -32768, 32767);
				*position = buf;
				position++;
			}
			out_bytes *= 2;
		}
		
		total_in_bytes += in_bytes;
		total_out_bytes += out_bytes;
		turn ++;
		
		/* finished or warning */
		if (decode_end != 0){
			break;
		}
	}
	
	// AAC dec. default endian is "big"
	if (gBF_AudioAAC_Dec_Endian == AAC_ENDIAN_BIG){
		BF_Audio_Data_ChangeEndian((BYTE*)outbuf, total_out_bytes);
	}
	
	/* Operand data making of the output result */
	param->in_data_size = total_in_bytes;
	param->out_data_size = total_out_bytes;
	
//### --- REMOVE_AAC END ---
	return ret_func;
}

UINT32 BF_AudioAAC_Dec_GetVersion( VOID )
{
	UINT32 version = 0;
//### --- REMOVE_AAC BEGIN ---

	version = AACD_getVersion();
//### --- REMOVE_AAC END ---
	return version;
}

