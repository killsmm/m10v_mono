/**
 * @file		audio_aac_dec.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _AUDIO_AAC_DEC_H_
#define _AUDIO_AAC_DEC_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
/* AAC Encoder Initialize Parameter*/
typedef struct {
	UINT8					format;				// AAC format
	UINT8					endian;				// Endian
	UINT32					audio_spec_cfg;		// Audio Specific Config
	UINT32					cfg_size;			// Size of "Audio Specific Config"
} T_AUDIO_AAC_DEC_INIT;

/* AAC Encode Parameter*/
typedef struct {
	UINT8					soft_gain;			// gain
	UINT32					in_data_addr;		// input data address
	UINT32					in_data_size;		// input data size
	UINT32					out_data_addr;		// output data address
	UINT32					out_data_size;		// output data size
	UINT32					out_sampling_rate;	// sampling rate of output data
	UINT32					out_bit_rate;		// bit rate of output data
	UINT32					out_channel;		// Number of output data channel
} T_AUDIO_AAC_DEC_PROC;

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/
extern UINT32 BF_AudioAAC_Dec_Init(const T_AUDIO_AAC_DEC_INIT* param);
extern UINT32 BF_AudioAAC_Dec_Process(T_AUDIO_AAC_DEC_PROC* param);
extern UINT32 BF_AudioAAC_Dec_GetVersion( VOID );


#endif	// _AUDIO_AAC_DEC_H_
