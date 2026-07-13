/**
 * @file		audio.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _AUDIO_H_
#define _AUDIO_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/


/* Return Definition */
#define D_AUDIO_OK					(0x00000000UL)
#define D_AUDIO_ERR					(0x00000001UL)
#define D_AUDIO_ERR_PARAM			(0x00000002UL)
#define D_AUDIO_ERR_TIMEOUT			(0x00000003UL)
#define D_AUDIO_WAV_HEADER_ERR		(0x00000004UL)
#define D_AUDIO_WAV_TIME_ERR		(0x00000005UL)
#define D_AUDIO_WAV_FORMAT_OVER		(0x0000000AUL)
#define D_AUDIO_END					(0x00000017UL)
#define D_AUDIO_BZ_PCM_END			(0x00000018UL)
#define D_AUDIO_ERR_OVERFLOW		(0x00000019UL)
#define D_AUDIO_ERR_INST_DISABLE	(0x0000001AUL)


#define D_AUDIO_ON					(1)
#define D_AUDIO_OFF					(0)

/* Sampling Rate */
#define D_AUDIO_SAMPLE_RATE_192000		(192000UL)
#define D_AUDIO_SAMPLE_RATE_96000		(96000UL)
#define D_AUDIO_SAMPLE_RATE_88200		(88200UL)
#define D_AUDIO_SAMPLE_RATE_64000		(64000UL)
#define D_AUDIO_SAMPLE_RATE_48000		(48000UL)
#define D_AUDIO_SAMPLE_RATE_44100		(44100UL)
#define D_AUDIO_SAMPLE_RATE_32000		(32000UL)
#define D_AUDIO_SAMPLE_RATE_24000		(24000UL)
#define D_AUDIO_SAMPLE_RATE_22050		(22050UL)
#define D_AUDIO_SAMPLE_RATE_16000		(16000UL)
#define D_AUDIO_SAMPLE_RATE_12000		(12000UL)
#define D_AUDIO_SAMPLE_RATE_11025		(11025UL)
#define D_AUDIO_SAMPLE_RATE_8000		(8000UL)

/* Bit Rate */
#define D_AUDIO_BIT_RATE_8000			(8000UL)
#define D_AUDIO_BIT_RATE_11025			(11025UL)
#define D_AUDIO_BIT_RATE_16000			(16000UL)
#define D_AUDIO_BIT_RATE_22050			(22050UL)
#define D_AUDIO_BIT_RATE_32000			(32000UL)
#define D_AUDIO_BIT_RATE_40000			(40000UL)
#define D_AUDIO_BIT_RATE_48000			(48000UL)
#define D_AUDIO_BIT_RATE_56000			(56000UL)
#define D_AUDIO_BIT_RATE_64000			(64000UL)
#define D_AUDIO_BIT_RATE_80000			(80000UL)
#define D_AUDIO_BIT_RATE_96000			(96000UL)
#define D_AUDIO_BIT_RATE_112000			(112000UL)
#define D_AUDIO_BIT_RATE_128000			(128000UL)
#define D_AUDIO_BIT_RATE_160000			(160000UL)
#define D_AUDIO_BIT_RATE_192000			(192000UL)
#define D_AUDIO_BIT_RATE_224000			(224000UL)
#define D_AUDIO_BIT_RATE_256000			(256000UL)
#define D_AUDIO_BIT_RATE_320000			(320000UL)
#define D_AUDIO_BIT_RATE_384000			(384000UL)


/*----------------------------------------------------------------------*/
/* Event Flag 															*/
/*----------------------------------------------------------------------*/

/* FID_AUDIO_CAP */
// Input
#define FLG_AUDIO_CAP_IN0_INIT				(0x00000001UL)	// init
#define FLG_AUDIO_CAP_IN1_INIT				(0x00000002UL)	// init
#define FLG_AUDIO_CAP_IN0_START				(0x00000004UL)	// start
#define FLG_AUDIO_CAP_IN1_START				(0x00000008UL)	// start
#define FLG_AUDIO_CAP_IN0_STOP				(0x00000010UL)	// stop
#define FLG_AUDIO_CAP_IN1_STOP				(0x00000020UL)	// stop
#define FLG_AUDIO_CAP_IN0_PLUGIN			(0x00000040UL)	// plug-in1 for cap ch0
#define FLG_AUDIO_CAP_IN1_PLUGIN			(0x00000080UL)	// plug-in1 for cap ch0
#define FLG_AUDIO_CAP_IN0_PCM_RCV			(0x00000100UL)	// PCM Receive0
#define FLG_AUDIO_CAP_IN1_PCM_RCV			(0x00000200UL)	// PCM Receive1
#define FLG_AUDIO_CAP_IN0_PCM_RCV_2ND		(0x00000400UL)	// PCM Receive0 for 4ch Input
#define FLG_AUDIO_CAP_IN1_PCM_RCV_2ND		(0x00000800UL)	// PCM Receive1 for 4ch Input
#define FLG_AUDIO_CAP_IN_STATE_CHANGE		(0x01000000UL)	// response
#define FLG_AUDIO_CAP_IN_STATE_ERR			(0x02000000UL)	// error response

/* FID_AUDIO_OUT */
// Output
#define FLG_AUDIO_OUT_OUT0_INIT				(0x00000001UL)	// init
#define FLG_AUDIO_OUT_OUT1_INIT				(0x00000002UL)	// init
#define FLG_AUDIO_OUT_OUT0_START			(0x00000004UL)	// start
#define FLG_AUDIO_OUT_OUT1_START			(0x00000008UL)	// start
#define FLG_AUDIO_OUT_OUT0_STOP				(0x00000010UL)	// stop
#define FLG_AUDIO_OUT_OUT1_STOP				(0x00000020UL)	// stop
#define FLG_AUDIO_OUT_OUT_PCM_SND0			(0x00000040UL)	// PCM Send0
#define FLG_AUDIO_OUT_OUT_PCM_SND1			(0x00000080UL)	// PCM Send1
#define FLG_AUDIO_OUT_OUT_PCM_SND0_2ND		(0x00000100UL)	// PCM Send0
#define FLG_AUDIO_OUT_OUT_PCM_SND1_2ND		(0x00000200UL)	// PCM Send1
#define FLG_AUDIO_OUT_BZ_STOP_NTFY0			(0x00001000UL)	// Buzzer Stop Notification0
#define FLG_AUDIO_OUT_BZ_STOP_NTFY1			(0x00002000UL)	// Buzzer Stop Notification1
#define FLG_AUDIO_OUT_OUT_STATE_CHANGE		(0x01000000UL)	// response
#define FLG_AUDIO_OUT_OUT_STATE_ERR			(0x02000000UL)	// error response

/* FID_AUDIO_ENCDEC */
#define FLG_AUDIO_ENCDEC_ENC0_INIT			(0x00000001UL)	// init encoder
#define FLG_AUDIO_ENCDEC_ENC1_INIT			(0x00000002UL)	// init encoder
#define FLG_AUDIO_ENCDEC_ENC0_TX_DATA		(0x00000004UL)	// transfer stream data
#define FLG_AUDIO_ENCDEC_ENC1_TX_DATA		(0x00000008UL)	// transfer stream data
#define FLG_AUDIO_ENCDEC_ENC0_START			(0x00000010UL)	// start encode0
#define FLG_AUDIO_ENCDEC_ENC1_START			(0x00000020UL)	// start encode1
#define FLG_AUDIO_ENCDEC_ENC0_STOP			(0x00000040UL)	// stop encode
#define FLG_AUDIO_ENCDEC_ENC1_STOP			(0x00000080UL)	// stop encode
#define FLG_AUDIO_ENCDEC_DEC_INIT			(0x00010000UL)	// init decoder
#define FLG_AUDIO_ENCDEC_DEC_DEC_CB			(0x00020000UL)	// decoder callback
#define FLG_AUDIO_ENCDEC_DEC_REQ_STRM		(0x00040000UL)	// request audio stream data
#define FLG_AUDIO_ENCDEC_DEC_START			(0x00080000UL)	// start decode
#define FLG_AUDIO_ENCDEC_DEC_STOP			(0x00100000UL)	// stop decode
#define FLG_AUDIO_ENCDEC_DEC_STOP_NTFY		(0x00200000UL)	// decode stop notification

/* PCM Valid Bit */
#define D_AUDIO_PCM_8_BIT					(8U)				// 8bit pcm
#define D_AUDIO_PCM_16_BIT					(16U)			// 16bit pcm
#define D_AUDIO_PCM_24_BIT					(24U)			// 24bit pcm

/* I2S Data Cycle */
#define D_AUDIO_I2S_CYCLE_32FS				(32U)			// 32FS
#define D_AUDIO_I2S_CYCLE_64FS				(64)			// 64FS


/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/
typedef struct {
	UINT32	event_flag;
	VOID	(*process)(VOID);
} T_AUDIO_TASK_PROCESS;

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/

/* FJ Function */
extern FJ_ERR_CODE fj_audio_src_open( E_FJ_AUDIO_SRC_ENUM audio_src, UCHAR* audio_src_id );
extern FJ_ERR_CODE fj_audio_src_close( UCHAR audio_src_id );
extern FJ_ERR_CODE fj_audio_src_connect( UCHAR audio_src_id, UCHAR audio_capture_id );
extern FJ_ERR_CODE fj_audio_src_disconnect( UCHAR audio_src_id, UCHAR audio_capture_id );

extern FJ_ERR_CODE fj_audio_capture_open( E_FJ_AUDIO_CAPTURE_ENUM audio_cap,  UCHAR* audio_capture_id );
extern FJ_ERR_CODE fj_audio_capture_close( UCHAR audio_capture_id );
extern FJ_ERR_CODE fj_audio_capture_connect_out( UCHAR audio_capture_id, UCHAR audio_out_id );
extern FJ_ERR_CODE fj_audio_capture_disconnect_out( UCHAR audio_capture_id, UCHAR audio_out_id );
extern FJ_ERR_CODE fj_audio_capture_connect_enc( UCHAR audio_capture_id, UCHAR audio_enc_id );
extern FJ_ERR_CODE fj_audio_capture_disconnect_enc( UCHAR audio_capture_id, UCHAR audio_enc_id );
extern FJ_ERR_CODE fj_audio_capture_config( UCHAR audio_capture_id, T_FJ_AUDIO_CAPTURE_CFG* audio_capture_config );
extern FJ_ERR_CODE fj_audio_capture_sample_config( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_SMPL_CFG* audio_cap_smpl_config);
extern FJ_ERR_CODE fj_audio_capture_set_volume( UCHAR audio_capture_id, UINT8 volume );
extern FJ_ERR_CODE fj_audio_capture_start( UCHAR audio_capture_id );
extern FJ_ERR_CODE fj_audio_capture_stop( UCHAR audio_capture_id );
extern FJ_ERR_CODE fj_audio_capture_enable_plugin1( UCHAR audio_capture_id, UCHAR enable );
extern FJ_ERR_CODE fj_audio_capture_complete_plugin1( UCHAR audio_capture_id, UCHAR free_area_index );

extern FJ_ERR_CODE fj_audio_out_open( E_FJ_AUDIO_OUT_ENUM audio_out,  UCHAR* audio_out_id );
extern FJ_ERR_CODE fj_audio_out_close( UCHAR audio_out_id );
extern FJ_ERR_CODE fj_audio_out_forceclose( UCHAR audio_out_id );
extern FJ_ERR_CODE fj_audio_out_config( UCHAR audio_out_id, T_FJ_AUDIO_OUT_CFG* audio_out_config );
extern FJ_ERR_CODE fj_audio_out_set_volume( UCHAR audio_out_id, UINT8 volume );
extern FJ_ERR_CODE fj_audio_out_direct_load_wav( UINT32 addr, ULONG size, UINT32* wav_id );
extern FJ_ERR_CODE fj_audio_out_direct_unload_wav( UINT32 wav_id );
extern FJ_ERR_CODE fj_audio_out_direct_play_wav( UCHAR audio_out_id, UINT32 wav_id );
extern FJ_ERR_CODE fj_audio_out_direct_stop_wav( UCHAR audio_out_id, UINT32 wav_id );
extern FJ_ERR_CODE fj_audio_out_direct_control_wav( UCHAR audio_out_id, E_FJ_AUDIO_WAV_CTL control );

extern FJ_ERR_CODE fj_audio_encoder_open( E_FJ_AUDIO_ENC_ENUM audio_enc, UCHAR* audio_enc_id );
extern FJ_ERR_CODE fj_audio_encoder_close( UCHAR audio_enc_id );
extern FJ_ERR_CODE fj_audio_encoder_free_index( UCHAR audio_enc_id, UINT32 free_astream_index, BOOL last_free_flg );
extern FJ_ERR_CODE fj_audio_encoder_config( UCHAR audio_enc_id, UINT32 wConfig, UINT32* wOperation );
extern FJ_ERR_CODE fj_audio_encoder_setup( UCHAR audio_enc_id );
extern FJ_ERR_CODE fj_audio_encoder_start( UCHAR audio_enc_id );
extern FJ_ERR_CODE fj_audio_encoder_stop( UCHAR audio_enc_id );
extern FJ_ERR_CODE fj_audio_encoder_startmute( UCHAR audio_enc_id );
extern FJ_ERR_CODE fj_audio_encoder_stopmute( UCHAR audio_enc_id );

extern FJ_ERR_CODE fj_audio_decoder_open( E_FJ_AUDIO_DEC_ENUM audio_dec, UCHAR* audio_dec_id );
extern FJ_ERR_CODE fj_audio_decoder_close( UCHAR audio_dec_id );
extern FJ_ERR_CODE fj_audio_decoder_connect( UCHAR audio_dec_id, UCHAR audio_out_id );
extern FJ_ERR_CODE fj_audio_decoder_disconnect( UCHAR audio_dec_id, UCHAR audio_out_id );
extern FJ_ERR_CODE fj_audio_decoder_init( UCHAR audio_dec_id );
extern FJ_ERR_CODE fj_audio_decoder_config( UCHAR audio_dec_id, T_FJ_AUDIO_DEC_CONFIG_PARAM* audio_dec_config );
extern FJ_ERR_CODE fj_audio_decoder_set_audio_stream( UCHAR audio_dec_id, UINT32 store_num, T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM* audio_stream );
extern FJ_ERR_CODE fj_audio_decoder_start_decode( UCHAR audio_dec_id );
extern FJ_ERR_CODE fj_audio_decoder_stop_decode( UCHAR audio_dec_id );
extern FJ_ERR_CODE fj_audio_decoder_start_output( UCHAR audio_dec_id );
extern FJ_ERR_CODE fj_audio_decoder_stop_output( UCHAR audio_dec_id );
extern FJ_ERR_CODE fj_audio_decoder_mem_allocate( UCHAR audio_dec_id, UINT32* size, UINT32* address );

// for VideoPlayback
extern FJ_ERR_CODE fj_videoplayback_audioinit( VOID );
extern FJ_ERR_CODE fj_videoplayback_setaudiostream( UINT32 store_num, T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM* audio_stream_param );
extern FJ_ERR_CODE fj_videoplayback_startaudiodecode( VOID );
extern FJ_ERR_CODE fj_videoplayback_startaudiooutput( VOID );
extern FJ_ERR_CODE fj_videoplayback_stopaudiooutput( VOID );
extern FJ_ERR_CODE fj_videoplayback_stopaudiodecode( VOID );


/* BF Function */

extern VOID BF_Audio_Print_Connect_Info( VOID );


#endif		//_AUDIO_H_
