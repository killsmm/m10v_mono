/**
 * @file		movlib.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */
#ifndef    _LIB_MOV_H_
#define    _LIB_MOV_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "fs_if.h"
#include "ddim_typedef.h"
//#include "fj_common.h"

/********************************** error code *************************/
#define   MOV_PARSE_OK		0x00000000	  // Parse OK
#define	MOV_ERR_READ        	0x00000001    // read error
#define   MOV_ERR_MEM         	0x00000002    // memory access error
#define   MOV_ERR_NOT_RIFF    	0x00000003    // not RIFF
#define   MOV_ERR_NOT_MOV     	0x00000004    // not MOV file
#define 	MOV_ERR_BROKEN      	0x00000005    // broken MOV file
#define   MOV_ERR_SEEK		0x00000006    // seek error
#define    MOV_PARSE_FAIL		0x00000007	  // Parse FAIL
#define 	MOVIF_KEYFRAME 		0x00000010L    // indicate a keyfram
// chunk size must be aligned even
#define    	ALIGN_EVEN(x)    x = (((x) & 0x01) ? ((x)+1) : (x))

/***************************** typedef ***********************/
//typedef    unsigned char        FOURCC[4];
//typedef    unsigned char        FOURCC1[5];
//typedef    unsigned short       WORD;
//typedef    unsigned int       	DWORD ;
//typedef    __int64  DWORDLONG;       //unsigned long long
//typedef    unsigned long 	    DWORDLONG;

typedef enum {
 	ATOM_INVALID = 0,
 	ATOM_FTYP,
	ATOM_MOOV,
 	ATOM_MVHD,
 	ATOM_TRAK,
 	ATOM_TKHD,
 	ATOM_EDTS,
 	ATOM_MDIA,
 	ATOM_MDHD,
 	ATOM_HDLR,
 	ATOM_MINF,
 	ATOM_VMHD,
 	ATOM_DINF,
 	ATOM_DREF,
 	ATOM_ALIS,
 	ATOM_STBL,
 	ATOM_STSD,
 	ATOM_AVC1,
 	ATOM_STTS,
 	ATOM_STSC,
 	ATOM_STSZ,
 	ATOM_STCO,
 	ATOM_STSS,
 	ATOM_TWOS,
}MOV_ATOM_TYPE;

typedef enum {
 	MOV_MINF_VIDEO = 0,
 	MOV_MINF_SOUND,
}MOV_MINF_STREAM_TYPE;


 /*********************************customarized defines*************************************/
typedef struct _MOVprop {
	unsigned int dwWidth;
	unsigned int dwHeight;
	unsigned int dwDuration;
	unsigned int dwSampleRate;
	unsigned short  dwNumberOfChannels;
 	unsigned int dwAudioBitRate;
}MOVPROP;

/**************************** MOV object ***************************/
typedef struct _MOV {
 	char    	*file_name;    // MOV file's name
 	int         fd;                // MOV file's descriptor 
}MOV;

/***************************** real interface ***************************/
//int MOV_parse(char * file,MOVPROP *oMOVProp);
int mov_parse(FW_CHAR * file,MOVPROP *oMOVProp);

/************ constructor & deconstructor ******************/
MOV* mov_new(char * file);
void mov_del(void);

#ifdef __cplusplus
 };
#endif    // __cplusplus


#endif    //_LIB_MOV_H_
