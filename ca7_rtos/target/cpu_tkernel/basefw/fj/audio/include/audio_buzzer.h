/**
 * @file		audio_buzzer.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _AUDIO_BUZZER_H_
#define _AUDIO_BUZZER_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/

extern INT32 BF_AudioBuzzer_DirectLoadWav( UINT32 addr, UINT32 size, UINT32* wav_id );
extern INT32 BF_AudioBuzzer_DirectUnloadWav( UINT32 wav_id );
extern INT32 BF_AudioBuzzer_DirectPlayWav( UCHAR audio_out_id, UINT32 wav_id );
extern INT32 BF_AudioBuzzer_DirectStopWav( UCHAR audio_out_id, UINT32 wav_id );
extern INT32 BF_AudioBuzzer_DirectControlWav( UCHAR audio_out_id, E_FJ_AUDIO_WAV_CTL control );

extern VOID BF_AudioBuzzer_Set_WavFile_Addr( UINT32 addr );
extern UINT32 BF_AudioBuzzer_Get_WavFile_Addr( VOID );
extern VOID BF_AudioBuzzer_Clear_WavFile_Addr( VOID );

extern INT32 BF_AudioBuzzer_Renov_Buff( UINT8 audio_out_ch );
extern UINT32 BF_AudioBuzzer_Get_BuffAddr( UINT8 audio_out_ch );
extern UINT32 BF_AudioBuzzer_Get_BuffSize( UINT8 audio_out_ch );
extern UINT8 BF_AudioBuzzer_Get_Buff_BitsPerSample( UINT8 audio_out_ch );
extern UINT8 BF_AudioBuzzer_Get_Buff_Channel( UINT8 audio_out_ch );

// for wav lib
extern INT32 wav_write( VOID *buf, ULONG bytes );
extern INT32 wav_read( VOID *buf, ULONG bytes );
extern INT32 wav_seek( LONG pos );
extern INT32 wav_seek_cur( LONG pos );

#endif	//_AUDIO_BUZZER_H_
