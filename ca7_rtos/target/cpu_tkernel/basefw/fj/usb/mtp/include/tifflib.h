/**
 * @file		tifflib.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#ifndef    _tiff_LIB_H_
 #define    _tiff_LIB_H__

 #ifdef __cplusplus
     extern "C" {
 #endif

#include "fs_if.h"
#include "ddim_typedef.h"
//#include "fj_common.h"


//typedef    unsigned short      WORD;
//typedef    unsigned int        DWORD;

typedef struct _tiffprop{

    unsigned int  dwWidth;
	unsigned int  dwHeight;
	unsigned short   wImageBitDepth;    

}TIFFPROP;


int tiff_parse(char * file,TIFFPROP* poTiffProp);



 #ifdef __cplusplus
     };
 #endif    // __cplusplus


 #endif    //_tiff_LIB_H_
