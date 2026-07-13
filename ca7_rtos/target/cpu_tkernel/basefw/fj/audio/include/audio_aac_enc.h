/**
 * @file		audio_aac_enc.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _AUDIO_AAC_ENC_H_
#define _AUDIO_AAC_ENC_H_
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_AUDIO_AAC_ENC_NUM		(2)

#define D_AUDIO_AAC_ENC_ID_0	(0)
#define D_AUDIO_AAC_ENC_ID_1	(1)

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
/* AAC Encoder Initialize Parameter*/
typedef struct {
	UINT8					aac_id;				// ID
	UINT8					format;				// AAC format
	UINT8					endian;				// Endian
	UINT8					channel;			// AAC ChannelType
	UINT32					smpl_rate;			// Sampling Rate
	UINT32					bit_rate;			// Bit Rate
	UINT32					audio_spec_cfg;		// Audio Specific Config
} T_AUDIO_AAC_ENC_INIT;

/* AAC Encode Parameter*/
typedef struct {
	UINT8					aac_id;				// ID
	UINT8					soft_gain;			// gain
	UINT32					in_data_addr;		// input data address
	UINT32					in_data_size;		// input data size
	UINT32					out_data_addr;		// output data address
	UINT32					out_data_size;		// output data size
} T_AUDIO_AAC_ENC_PROC;

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/

extern UINT32 BF_AudioAAC_Enc_Init(T_AUDIO_AAC_ENC_INIT* param);
extern UINT32 BF_AudioAAC_Enc_Process(T_AUDIO_AAC_ENC_PROC* param);
extern UINT32 BF_AudioAAC_Enc_GetVersion( VOID );

#endif	//_AUDIO_AAC_ENC_H_
