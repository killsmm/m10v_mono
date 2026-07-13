/**
 * @file		audio_out.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _AUDIO_OUT_H_
#define _AUDIO_OUT_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_AUDIO_OUT_INST_NUM				(2)

#define D_AUDIO_OUT_STATUS_INIT				(1)
#define D_AUDIO_OUT_STATUS_START			(2)
#define D_AUDIO_OUT_STATUS_STOP				(0)

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/

/**	Audio Source Instance */
typedef struct {
	UINT8					status;					// Status
	UCHAR					audio_out_id;			// AudioOutID
	UINT8					enable;					// enable/disable
	E_FJ_AUDIO_OUT_ENUM		type;					// type
	UINT8					buzzer_flg;				// Buzzer Flag[1: Output Buzzer / 0: Not Output Buzzer]
	UINT8					cap_flg;				// Capture Flag[1: Output Capture / 0: Not Capture Buzzer]
	UINT8					dec_flg;				// Decoder Flag[1: Output Capture / 0: Not Capture Buzzer]
	E_FJ_AUDIO_WAV_CTL		bz_mode;				// Buzzer Control Mode
	UCHAR					connect_cap_id;			// Connected AudioCaptureID
	UCHAR					connect_dec_id;			// Connected AudioDecoderID
	UINT8					set_cfg_flg;			// Flag of Configuration set/not set
	E_FJ_AUDIO_OUT_PORT_ENUM	out_port;			// Input Port Configuration
	T_FJ_AUDIO_I2S_CFG		i2s_cfg; 				// I2S Configuration Information
	UINT32					in_smpl_rate;			// Sampling Rate of input data
	UINT32					out_smpl_rate;			// Sampling Rate of output data
	UINT8					in_ch_type;				// Channel Type of input data
	UINT8					out_ch_type;			// Channel Type of output data
	UINT8					dma_id[2];				// DMA ID
	UINT32					output_count;			// input frame number
} T_AUDIO_OUT_INSTANCE;

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/

extern INT32 BF_AudioOut_Open( E_FJ_AUDIO_OUT_ENUM audio_cap, UCHAR* audio_out_id );
extern INT32 BF_AudioOut_Close( UCHAR audio_out_id );
extern INT32 BF_AudioOut_ForceClose( UCHAR audio_out_id );
extern INT32 BF_AudioOut_Config( UCHAR audio_out_id, T_FJ_AUDIO_OUT_CFG* audio_out_config );
extern INT32 BF_AudioOut_Start( UCHAR audio_out_id );
extern INT32 BF_AudioOut_Start_Capture( UCHAR audio_out_id );
extern INT32 BF_AudioOut_Start_Buzzer( UCHAR audio_out_id );
extern INT32 BF_AudioOut_Stop( UCHAR audio_out_id );
extern INT32 BF_AudioOut_Stop_Buzzer( UCHAR audio_out_id );
extern INT32 BF_AudioOut_Start_Decoder( UCHAR audio_out_id );
extern INT32 BF_AudioOut_Stop_Decoder( UCHAR audio_out_id );
extern INT32 BF_AudioOut_ConnectCap( UCHAR audio_out_id, UCHAR audio_cap_id );
extern INT32 BF_AudioOut_DisconnectCap( UCHAR audio_out_id, UCHAR audio_cap_id );
extern INT32 BF_AudioOut_ConnectDec( UCHAR audio_out_id, UCHAR audio_dec_id );
extern INT32 BF_AudioOut_DisconnectDec( UCHAR audio_out_id, UCHAR audio_dec_id );
extern INT32 BF_AudioOut_SetChannelType( UCHAR audio_out_id, UINT8 ch_type );
extern INT32 BF_AudioOut_SetSmplRate( UCHAR audio_out_id, UINT32 smpl_rate );
extern UINT8 BF_AudioOut_Get_Status( UCHAR audio_out_id );
extern UINT8 BF_AudioOut_Get_ConfigSetFlag( UCHAR audio_out_id );
extern INT32 BF_AudioOut_Set_BuzzerMode( UCHAR audio_out_id, E_FJ_AUDIO_WAV_CTL mode );
extern E_FJ_AUDIO_OUT_PORT_ENUM BF_AudioOut_Get_Config_I2sCh( UCHAR audio_out_id );
extern VOID BF_AudioOut_SetTestMode( UCHAR audio_out_id, UINT8 mode );

extern VOID Audio_Out_Process( INT32 stacd );
extern VOID BF_AudioOut_TaskStart( VOID );
extern VOID BF_AudioOut_TaskStop( VOID );

extern VOID BF_AudioOut_OutputInit( UINT8 out_ch );
extern VOID BF_AudioOut_OutputStart( UINT8 out_ch );
extern VOID BF_AudioOut_OutputStop( UINT8 out_ch );
extern VOID BF_AudioOut_PcmSend( UINT8 out_ch );
extern VOID BF_AudioOut_NotifyBZStop( UINT8 out_ch );
extern INT32 BF_AudioOut_Get_OutCh( UCHAR audio_out_id, UINT8* out_ch );
extern INT32 BF_AudioOut_Set_Volume( UCHAR audio_out_id, UINT8 volume );

extern T_AUDIO_OUT_INSTANCE BF_AudioOut_Get_InstanceInfo( UINT8 index );

#endif		//_AUDIO_OUT_H_
