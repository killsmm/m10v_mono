/**
 * @file		audio_pdm.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _AUDIO_PDM_H_
#define _AUDIO_PDM_H_


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_AUDIO_PDM_CH0			(0)
#define D_AUDIO_PDM_CH1			(1)
#define D_AUDIO_PDM_CH_NUM		(2)

#define D_AUDIO_PDM_STAUS_INIT		(0)
#define D_AUDIO_PDM_STAUS_START		(1)
#define D_AUDIO_PDM_STAUS_STOP		(2)

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/
extern INT32 BF_Audio_Pdm_Init_PdmMacro( VOID );
extern INT32 BF_Audio_Pdm_Open( UINT8 dev_ch );
extern INT32 BF_Audio_Pdm_Close( UINT8 dev_ch );
extern INT32 BF_Audio_Pdm_Config( UINT8 dev_ch, UINT32 freq_smpl );
extern INT32 BF_Audio_Pdm_Start( UINT8 dev_ch );
extern INT32 BF_Audio_Pdm_Stop( UINT8 dev_ch );
extern UINT8 BF_Audio_Pdm_GetStatus( UINT8 dev_ch );

#endif		//_AUDIO_PDM_H_

