/**
 * @file		audio_encdec.h
 * @brief		audio function api
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#ifndef _AUDIO_ENCDEC_H_
#define _AUDIO_ENCDEC_H_


/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Structure  															*/
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Function  															*/
/*----------------------------------------------------------------------*/

extern VOID Audio_EncDec_Process( INT32 stacd );
extern VOID BF_AudioEncDec_TaskStart( VOID );
extern VOID BF_AudioEncDec_TaskStop( VOID );

extern INT32 BF_AudioEncDec_Wai_Sem( VOID );
extern INT32 BF_AudioEncDec_Sig_Sem( VOID );

#endif		//_AUDIO_ENCDEC_H_
