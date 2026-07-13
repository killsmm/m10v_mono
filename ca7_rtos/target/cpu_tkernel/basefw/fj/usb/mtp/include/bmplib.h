/**
 * @file		bmplib.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#ifndef    _BMP_LIB_H_
#define    _BMP_LIB_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "fs_if.h"
#include "ddim_typedef.h"
//#include "fj_common.h"

typedef struct _bmpprop{
	unsigned int dwWidth;
	unsigned int dwHeight;
	unsigned short  wImageBitDepth;    
}BMPPROP;

int  bmp_parse(char * file,BMPPROP* poBmpProp);

#ifdef __cplusplus
 };
#endif    // __cplusplus


#endif    //_BMP_LIB_H_
