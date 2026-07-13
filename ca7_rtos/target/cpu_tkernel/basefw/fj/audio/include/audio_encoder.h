/**
 * @file		audio_encoder.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _AUDIO_ENCODER_H_
#define _AUDIO_ENCODER_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_AUDIO_ENC_INST_NUM				(2)

#define D_AUDIO_ENC_STATUS_INIT				(1)
#define D_AUDIO_ENC_STATUS_START_PROC		(2)
#define D_AUDIO_ENC_STATUS_START			(3)
#define D_AUDIO_ENC_STATUS_STOP_PROC		(4)
#define D_AUDIO_ENC_STATUS_START_PROC_STOP	(5)
#define D_AUDIO_ENC_STATUS_STOP				(0)

#define D_AUDIO_ENC_CMPR_AAC				(1)
#define D_AUDIO_ENC_CMPR_HEAAC				(2)
#define D_AUDIO_ENC_CMPR_PCM				(3)

#define D_AUDIO_ENC_MUTE					(1)
#define D_AUDIO_ENC_DEMUTE					(0)

#define D_AUDIO_ENC_CH_0					(0)
#define D_AUDIO_ENC_CH_1					(1)

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/

/**	Audio Source Instance */
typedef struct {
	UINT8					status;					// Status
	UCHAR					audio_enc_id;			// AudioEncoderID
	UINT8					enable;					// enable/disable
	E_FJ_AUDIO_ENC_ENUM		type;					// type
	UINT8					enc_enable;				// encoder enable/disable
	UINT8					compress_type;			// CompressType(AAC, HE-AAC, PCM)
	UCHAR					connect_cap_id;			// Connected AudioCaptureID
	UINT32					smpl_rate;				// Sampling Rate of input data
	UINT32					bit_rate;				// Sampling Rate of output data
	UINT8					ch_type;				// Number of Channels
	UINT32					enc_count;				// input frame number
	UINT32					offset;					// offset of Encoded Samples
	UINT8					mute;					// 1:mute 0:demute
} T_AUDIO_ENC_INSTANCE;

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/

extern INT32 BF_AudioEnc_Open( E_FJ_AUDIO_ENC_ENUM audio_enc, UCHAR* audio_enc_id );
extern INT32 BF_AudioEnc_Close( UCHAR audio_enc_id );
extern INT32 BF_AudioEnc_Config( UCHAR audio_enc_id, T_FJ_AUDIO_AAC_CONFIG_PARAM* audio_enc_config );
extern INT32 BF_AudioEnc_Setup( UCHAR audio_enc_id );

extern INT32 BF_AudioEnc_Start( UCHAR audio_enc_id );
extern INT32 BF_AudioEnc_Stop( UCHAR audio_enc_id );
extern INT32 BF_AudioEnc_FreeIndex( UCHAR audio_enc_id, UINT32 free_astream_index, BOOL last_free_flg );
extern INT32 BF_AudioEnc_StartMute( UCHAR audio_enc_id );
extern INT32 BF_AudioEnc_StopMute( UCHAR audio_enc_id );
extern VOID BF_AudioEnc_SetTestMode( UINT8 mode );
extern UINT8 BF_AudioEnc_GetTestMode( VOID );

extern INT32 BF_AudioEnc_Connect( UCHAR audio_enc_id, UCHAR audio_cap_id );
extern INT32 BF_AudioEnc_Disconnect( UCHAR audio_enc_id, UCHAR audio_cap_id );

extern VOID BF_AudioEnc_Init_Encoder( UINT8 enc_ch );
extern VOID BF_AudioEnc_StartEncode( UINT8 enc_ch );
extern VOID BF_AudioEnc_StopEncode( UINT8 enc_ch );
extern VOID BF_AudioEnc_TxStreamData( UINT8 enc_ch );

extern INT32 BF_AudioEnc_Request_Encode( UCHAR audio_enc_id, UINT8 enc_cnt );

extern T_AUDIO_ENC_INSTANCE BF_AudioEnc_Get_InstanceInfo( UINT8 index );

#endif		//_AUDIO_ENCODER_H_
