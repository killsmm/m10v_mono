/**
 * @file		audio_decoder.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _AUDIO_DECODER_H_
#define _AUDIO_DECODER_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_AUDIO_DEC_STATUS_INIT				(1)
#define D_AUDIO_DEC_STATUS_START			(2)
#define D_AUDIO_DEC_STATUS_STOP_PROC		(3)
#define D_AUDIO_DEC_STATUS_STOP				(0)

#define D_AUDIO_DEC_CMPR_AAC				(1)
#define D_AUDIO_DEC_CMPR_PCM				(2)

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/

/**	Audio Decoder Instance */
typedef struct {
	UINT8					status;					// Status
	UCHAR					audio_dec_id;			// AudioDecoderID
	UINT8					enable;					// enable/disable
	E_FJ_AUDIO_DEC_ENUM		type;					// type
	UINT8					dec_enable;				// decoder enable/disable
	UINT8					compress_type;			// CompressType(AAC, HE-AAC, PCM)
	UCHAR					connect_out_id[2];			// Connected AudioOut ID
	UINT32					smpl_rate;				// Sampling Rate of input data
	UINT8					ch_type;				// Number of Channels
	UINT32					dec_count;				// Input frame number
	UINT64					current_pts;			// Current PTS
	UINT32					offset;					// Offset
	UINT32					file_end;				// File End Flag
} T_AUDIO_DEC_INSTANCE;

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/

extern INT32 BF_AudioDec_Open( E_FJ_AUDIO_DEC_ENUM audio_dec, UCHAR* audio_dec_id );
extern INT32 BF_AudioDec_Close( UCHAR audio_dec_id );
extern INT32 BF_AudioDec_Connect( UCHAR audio_dec_id, UCHAR audio_out_id );
extern INT32 BF_AudioDec_Disconnect( UCHAR audio_dec_id, UCHAR audio_out_id );

extern INT32 BF_AudioDec_Init( UCHAR audio_dec_id );
extern INT32 BF_AudioDec_Config( UCHAR audio_dec_id, T_FJ_AUDIO_DEC_CONFIG_PARAM* audio_enc_config );
extern INT32 BF_AudioDec_SetAudioStream( UCHAR audio_dec_id, UINT32 store_num, T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM* audio_stream );

extern INT32 BF_AudioDec_Start( UCHAR audio_dec_id );
extern INT32 BF_AudioDec_Stop( UCHAR audio_dec_id );
extern INT32 BF_AudioDec_StartOutput( UCHAR audio_dec_id );
extern INT32 BF_AudioDec_StopOutput( UCHAR audio_dec_id );
extern INT32 BF_AudioDec_MemAllocate( UCHAR audio_dec_id, UINT32* size, UINT32* address );
extern INT32 BF_AudioDec_Request_Decode( UCHAR audio_dec_id, UCHAR audio_out_id);
extern INT32 BF_AudioDec_Renov_Buff( UCHAR audio_out_id );
extern UINT32 BF_AudioDec_Get_BuffAddr( UCHAR audio_out_id );
extern UINT32 BF_AudioDec_Get_BuffSize( UCHAR audio_out_id );

extern VOID BF_AudioDec_Init_Decode( VOID );
extern VOID BF_AudioDec_Start_Decode( VOID );
extern VOID BF_AudioDec_Stop_Decode( VOID );
extern VOID BF_AudioDec_Stop_Decode_Ntfy( VOID );
extern VOID BF_AudioDec_Ntfy_DecoderInfo( VOID );
extern VOID BF_AudioDec_Req_AudioStream( VOID );

extern T_AUDIO_DEC_INSTANCE BF_AudioDec_Get_InstanceInfo( VOID );


#endif		//_AUDIO_DECODER_H_
