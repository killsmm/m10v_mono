/**
 * @file		wavlib.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#ifndef    _LIB_WAV_H_
#define    _LIB_WAV_H_

//#include "itron.h"
//#include "realos.h"
#include "fs_if.h"
//#include "R_Header.h"
//#include "fj_common.h"



#ifdef __cplusplus
 extern "C" {
#endif

//typedef    unsigned int        DWORD ;
//typedef    unsigned short       WORD ;



// (2008/10/21) tony +
typedef struct _wavprop {
	unsigned int  dwDuration;
	unsigned int  dwSampleRate;
	unsigned short  dwNumberOfChannels;
	unsigned int dwAudioBitRate;
}WAVPROP;
// (2008/10/21) tony -



#ifdef __cplusplus
 };
#endif    // __cplusplus



#endif
