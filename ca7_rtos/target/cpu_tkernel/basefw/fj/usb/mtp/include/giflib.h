/**
 * @file		giflib.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#ifndef    _GIF_LIB_H_
#define    _GIF_LIB_H__

#ifdef __cplusplus
 extern "C" {
#endif


#include "fs_if.h"
#include "ddim_typedef.h"
//#include "fj_common.h"

typedef struct _gifprop{

unsigned short wWidth;
unsigned short wHeight;
char  cImageBitDepth;    

}GIFPROP;

int gif_parse(char* file,GIFPROP* poGIFProp);

#ifdef __cplusplus
 };
#endif    // __cplusplus


#endif    //_BMP_LIB_H_
