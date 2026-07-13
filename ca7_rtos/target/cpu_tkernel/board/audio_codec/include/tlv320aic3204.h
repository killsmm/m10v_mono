/**
 * @file		tlv320aic3204.h
 * @brief		tlv320aic3204 Driver Header
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _TLV320AIC3204_H_
#define _TLV320AIC3204_H_

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_AIC3204_OK		(0)
#define D_AIC3204_ERROR		(-1)


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/* AIC3204 State */
typedef enum{
	E_AIC3204_CLOSE = 0,
	E_AIC3204_OPEN
}E_AIC3204_STATE;

/* Audio Mode */
typedef enum{
	E_AIC3204_MODE_STOP = 0,
	E_AIC3204_MODE_PLAY,
	E_AIC3204_MODE_RECORD
}E_AIC3204_MODE;

/*  Audio volume type */
typedef enum{
	E_AIC3204_VOLUME_0	= 0,
	E_AIC3204_VOLUME_1,
	E_AIC3204_VOLUME_2,
	E_AIC3204_VOLUME_3,
	E_AIC3204_VOLUME_4,
	E_AIC3204_VOLUME_5,
	E_AIC3204_VOLUME_6,
	E_AIC3204_VOLUME_7,
	E_AIC3204_VOLUME_8,
	E_AIC3204_VOLUME_9,
	E_AIC3204_VOLUME_10,
	E_AIC3204_VOLUME_STOPPER
}E_AIC3204_VOLUME;

/* Audio Soft-Mute */
typedef enum{
	E_AIC3204_MUTE_ON = 0,
	E_AIC3204_MUTE_OFF,
	E_AIC3204_MUTE_STOPPER
}E_AIC3204_MUTE;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function Prototype													*/
/*----------------------------------------------------------------------*/
int AIC3204_Open( void );
int AIC3204_Close( void );

int AIC3204_PowerUp( void );
int AIC3204_PowerDown( void );

int AIC3204_Play_Start( void );
int AIC3204_Play_Stop( void );

int AIC3204_Record_Start( void );
int AIC3204_Record_Stop( void );

int AIC3204_Set_Volume( E_AIC3204_VOLUME vol );
int AIC3204_Set_Mute( E_AIC3204_MUTE mute );

unsigned char AIC3204_Is_Used( void );


//--------------------------------------------------------------------------------------------


#endif	// _TLV320AIC3204_H_
