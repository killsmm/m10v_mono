/**
 * @file		audio_capture.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _AUDIO_CAPTURE_H_
#define _AUDIO_CAPTURE_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

#define D_AUDIO_CAPTURE_INST_NUM				(2)

#define D_AUDIO_CAPTURE_STATUS_INIT				(1)
#define D_AUDIO_CAPTURE_STATUS_START			(2)
#define D_AUDIO_CAPTURE_STATUS_STOP				(0)

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/

/**	Audio Source Instance */
typedef struct {
	UINT8					status;					// Status
	UCHAR					audio_capture_id;		// AudioCaptureID
	UINT8					enable;					// enable/disable
	E_FJ_AUDIO_CAPTURE_ENUM	type;					// type
	UCHAR					connect_enc_id[2];		// Connected AudioEncoderID
	UCHAR					connect_out_id;			// Connected AudioOutID
	UINT8					out_start_flg;			// AudioOut Start Flag
	E_FJ_AUDIO_IN_PORT_ENUM	in_port;				// Input Port Configuration
	T_FJ_AUDIO_I2S_CFG		i2s_cfg; 				// I2S Configuration Information
	UINT8					plug_in1_enable;		// PlugIn1 enable/disable
	UINT32					in_smpl_rate;			// Input Sampling Rate
	UINT32					out_smpl_rate;			// Output Sampling Rate
	UINT8					in_ch_type;				// Input Channel Type
	UINT8					out_ch_type;			// Output Channel Type
	UINT8					in_out_addr_mode;		// 0:different, 1:same
	UINT8					dma_id[2];				// DMA ID
	UINT32					input_count;			// input frame number
} T_AUDIO_CAPTURE_INSTANCE;

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/

extern INT32 BF_AudioCapture_Open( E_FJ_AUDIO_CAPTURE_ENUM audio_cap, UCHAR* audio_capture_id );
extern INT32 BF_AudioCapture_Close( UCHAR audio_capture_id );
extern INT32 BF_AudioCapture_ConnectOut( UCHAR audio_capture_id, UCHAR audio_out_id );
extern INT32 BF_AudioCapture_DisconnectOut( UCHAR audio_capture_id, UCHAR audio_out_id );
extern INT32 BF_AudioCapture_ConnectEnc( UCHAR audio_capture_id, UCHAR audio_enc_id );
extern INT32 BF_AudioCapture_DisconnectEnc( UCHAR audio_capture_id, UCHAR audio_enc_id );
extern INT32 BF_AudioCapture_Config( UCHAR audio_capture_id, T_FJ_AUDIO_CAPTURE_CFG* audio_capture_config );
extern INT32 BF_AudioCapture_SampleConfig( UCHAR audio_capture_id, T_FJ_AUDIO_CAP_SMPL_CFG* audio_smpl_config );
extern INT32 BF_AudioCapture_Start( UCHAR audio_capture_id );
extern INT32 BF_AudioCapture_Stop( UCHAR audio_capture_id );
extern INT32 BF_AudioCapture_EnablePlugIn1( UCHAR audio_capture_id, UINT8 enable );
extern INT32 BF_AudioCapture_CompletePlugIn1( UCHAR audio_capture_id, UINT8 free_area_index );


// Task
extern VOID Audio_Cap_Process(INT32 stacd);
extern VOID BF_AudioCapture_TaskStart(VOID);
extern VOID BF_AudioCapture_TaskStop(VOID);


// Internal Process
extern VOID BF_AudioCapture_InputInit( UINT8 cap_ch );
extern VOID BF_AudioCapture_InputStart( UINT8 cap_ch );
extern VOID BF_AudioCapture_InputStop( UINT8 cap_ch );
extern VOID BF_AudioCapture_PcmReceive( UINT8 cap_ch );
extern VOID BF_AudioCapture_TxPlugIn1( UINT8 cap_ch );


// Buffer Control
extern INT32 BF_AudioCapture_Renov_Buff_Out( UCHAR audio_capture_id );
extern UINT32 BF_AudioCapture_Get_BuffAddr_Out( UCHAR audio_capture_id );
extern UINT32 BF_AudioCapture_Get_BuffSize_Out( UCHAR audio_capture_id );

extern UINT8 BF_AudioCapture_Get_Buff_InPos( UCHAR audio_capture_id );
extern VOID BF_AudioCapture_Set_EncStartBuffPos( UCHAR audio_capture_id, UCHAR audio_enc_id );
extern INT32 BF_AudioCapture_Renov_Buff_Enc( UCHAR audio_capture_id, UCHAR audio_enc_id );
extern UINT32 BF_AudioCapture_Get_BuffAddr_Enc( UCHAR audio_capture_id, UCHAR audio_enc_id );
extern UINT32 BF_AudioCapture_Get_BuffSize_Enc( UCHAR audio_capture_id, UCHAR audio_enc_id );
extern UINT32 BF_AudioCapture_Get_BuffBankCount_Enc( UCHAR audio_capture_id, UCHAR audio_enc_id );
extern UINT8 BF_AudioCapture_Get_Buff_BytePerSample( UCHAR audio_capture_id );

extern UINT8 BF_AudioCapture_Get_BuffChType( UCHAR audio_capture_id );
extern UINT16 BF_AudioCapture_Get_SampleOffset( UCHAR audio_capture_id );
extern VOID BF_AudioCapture_Clear_EncStartFlag( UCHAR audio_capture_id, UCHAR audio_enc_id);
extern VOID BF_AudioCapture_SetTestMode( UINT8 mode );
extern UINT8 BF_AudioCapture_Get_Status( UCHAR audio_capture_id );
extern INT32 BF_AudioCapture_Set_Volume( UCHAR audio_capture_id, UINT8 volume );

extern T_AUDIO_CAPTURE_INSTANCE BF_AudioCapture_Get_InstanceInfo( UINT8 index );


#endif		//_AUDIO_CAPTURE_H_
