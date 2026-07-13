/**
 * @file		audio_codec_wrapper.h
 * @brief		wrapper of audio codec driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _AUDIO_CODEC_WRAPPER_H_
#define _AUDIO_CODEC_WRAPPER_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Enumeration  														*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/

// for RK-Board
void AudioCodecWrapper_PowerUp( void );
void AudioCodecWrapper_PowerDown( void );
void AudioCodecWrapper_Play_Start( void );
void AudioCodecWrapper_Play_Stop( void );
void AudioCodecWrapper_Record_Start( void );
void AudioCodecWrapper_Record_Stop( void );
void AudioCodecWrapper_Set_Vol( unsigned char vol );

// for Other-Board
int AudioCodecWrapper_Hardware_Reset( void );
int AudioCodecWrapper_Setup_Pll( void );
int AudioCodecWrapper_I2s( void );
int AudioCodecWrapper_Analog_Init( void );
int AudioCodecWrapper_Int_Mic_Setup( void );
int AudioCodecWrapper_Ext_Mic_Setup( void );
int AudioCodecWrapper_Setup_Input( void );
int AudioCodecWrapper_Setup_Input( void );
int AudioCodecWrapper_Iir_Biquad_Filters( void );
int AudioCodecWrapper_Set_Mute( int mute );
int AudioCodecWrapper_Setup_Speaker_Playback( void );
int AudioCodecWrapper_Set_Speaker_Amp_Attenuation( unsigned int atten );
int AudioCodecWrapper_I2s_Setup( void );


#endif	// _AUDIO_CODEC_WRAPPER_H_
