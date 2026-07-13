/**
 * @file		audio_codec_wrapper.c
 * @brief		wrapper of audio codec driver
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "audio_codec_wrapper.h"
#include "tlv320aic3204.h"

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

// for rk
void AudioCodecWrapper_PowerUp( void )
{
	AIC3204_Open();
	AIC3204_PowerUp();
	AIC3204_Close();
}

void AudioCodecWrapper_PowerDown( void )
{
	AIC3204_Open();
	AIC3204_PowerDown();
	AIC3204_Close();
}

void AudioCodecWrapper_Play_Start( void )
{
	AIC3204_Open();
	AIC3204_Play_Start();
	AIC3204_Close();
}

void AudioCodecWrapper_Play_Stop( void )
{
	AIC3204_Open();
	AIC3204_Play_Stop();
	AIC3204_Close();
}

void AudioCodecWrapper_Record_Start( void )
{
	AIC3204_Open();
	AIC3204_Record_Start();
	AIC3204_Close();
}

void AudioCodecWrapper_Record_Stop( void )
{
	AIC3204_Open();
	AIC3204_Record_Stop();
	AIC3204_Close();
}

void AudioCodecWrapper_Set_Vol( unsigned char vol )
{
	AIC3204_Open();
	AIC3204_Set_Volume((E_AIC3204_VOLUME)vol);
	AIC3204_Close();
}


// for other
int AudioCodecWrapper_Hardware_Reset( void )
{
	return 0;
}

int AudioCodecWrapper_Setup_Pll( void )
{
	return 0;
}

int AudioCodecWrapper_I2s( void )
{
	return 0;
}

int AudioCodecWrapper_Analog_Init( void )
{
	return 0;
}

int AudioCodecWrapper_Int_Mic_Setup( void )
{
	return 0;
}

int AudioCodecWrapper_Ext_Mic_Setup( void )
{
	return 0;
}

int AudioCodecWrapper_Setup_Input( void )
{
	return 0;
}

int AudioCodecWrapper_Iir_Biquad_Filters( void )
{
	return 0;
}

int AudioCodecWrapper_Set_Mute( int mute )
{
	return 0;
}

int AudioCodecWrapper_Setup_Speaker_Playback( void )
{
	return 0;
}

int AudioCodecWrapper_Set_Speaker_Amp_Attenuation( unsigned int atten )
{
	return 0;
}

int AudioCodecWrapper_I2s_Setup( void )
{
	return 0;
}

