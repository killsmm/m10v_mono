/**
 * @file		jfiflib.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#ifndef    _JFIF_LIB_H_
 #define    _JFIF_LIB_H__

 #ifdef __cplusplus
     extern "C" {
 #endif

typedef struct _jfifprop{
	char  cImageBitDepth; 
	unsigned short wHeight;	
	unsigned short wWidth;
}JFIFPROP;

void jfif_parse(char* file,JFIFPROP* oJFIFProp);

 #ifdef __cplusplus
     };
 #endif    // __cplusplus


 #endif    //_JFIF_LIB_H_
