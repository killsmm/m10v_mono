/**
 * @file		mp3lib.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#ifndef    _mp3_LIB_H_
 #define    _mp3_LIB_H__

 #ifdef __cplusplus
     extern "C" {
 #endif


#include "fs_if.h"
#include "ddim_typedef.h"


typedef struct _mp3prop{
	float fMP3BitRate;
	int 	nDuration;
	float  fMP3SampleRate;
	int    nNumberOfChannels;
}MP3PROP;

 void  mp3_parse(FW_CHAR * file,MP3PROP *omp3Prop);



 #ifdef __cplusplus
     };
 #endif    // __cplusplus


 #endif    //_mp3_LIB_H_
