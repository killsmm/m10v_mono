/**
 * @file		avilib.h
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>ALL RIGHTS RESERVED, COPYRIGHT&copy; SOCIONEXT INCORPORATED 2015</I></B>
 */

#ifndef    _LIB_AVI_H_
#define    _LIB_AVI_H_

#ifdef __cplusplus
 extern "C" {
#endif


#include "fs_if.h"
#include "ddim_typedef.h"
//#include "fj_common.h"

/********************************** error code *************************/
#define   AVI_PARSE_OK		0x00000000	  // Parse OK
#define	AVI_ERR_READ        	0x00000001    // read error
#define   AVI_ERR_MEM         	0x00000002    // memory access error
#define   AVI_ERR_NOT_RIFF    	0x00000003    // not RIFF
#define   AVI_ERR_NOT_AVI     	0x00000004    // not AVI file
#define 	AVI_ERR_BROKEN      	0x00000005    // broken AVI file
#define   AVI_ERR_SEEK		0x00000006    // seek error
#define    AVI_PARSE_FAIL		0x00000007	  // Parse FAIL
#define 	AVIIF_KEYFRAME 		0x00000010L    // indicate a keyfram
// chunk size must be aligned even
#define    	ALIGN_EVEN(x)    x = (((x) & 0x01) ? ((x)+1) : (x))

/***************************** typedef ***********************/
typedef    unsigned char        FOURCC[4];
typedef    unsigned char        FOURCC1[5];
typedef    unsigned short       WORD;
typedef    unsigned int       	DWORD ;
//typedef    __int64  DWORDLONG;       //unsigned long long
typedef    unsigned long 	    DWORDLONG;

typedef enum {
 	FOURCC_UNDEF = 0,
	FOURCC_RIFF,
 	FOURCC_LIST,
 	FOURCC_JUNK,
 	FOURCC_AVI,
 	FOURCC_AVIX,
 	FOURCC_WAVE,
 	FOURCC_INFO,
 	FOURCC_avih,
 	FOURCC_hdrl,
 	FOURCC_movi,
 	FOURCC_idx1,
 	FOURCC_strl,
 	FOURCC_strh,
 	FOURCC_strf,
 	FOURCC_strd,
 	FOURCC_strn,
 	FOURCC_indx,
 	FOURCC_rec,
 	FOURCC_auds,
 	FOURCC_vids,
 	FOURCC_txts,
 	FOURCC_mids,     
 	// following fourcc are used to identify subchunks of INFO chunk,
 	// they are ignored in this library.
 	FOURCC_IARL,
 	FOURCC_IART,
 	FOURCC_ICMS,
 	FOURCC_ICMT,
 	FOURCC_ICOP,
 	FOURCC_ICRD,
 	FOURCC_ICRP,
 	FOURCC_IDIM,
 	FOURCC_IDPI,
 	FOURCC_IENG,
 	FOURCC_IGNR,
 	FOURCC_IKEY,
 	FOURCC_ILGT,
 	FOURCC_IMED,
 	FOURCC_INAM,
 	FOURCC_IPLT,
 	FOURCC_IPRD,
 	FOURCC_ISBJ,
 	FOURCC_ISFT,
 	FOURCC_ISHP,
 	FOURCC_ISRC,
 	FOURCC_ISRF,
 	FOURCC_ITCH,
 	FOURCC_ISMP,
 	FOURCC_IDIT,
}AVI_FOURCC_TYPE;

 // TWOCC
 /*
 typedef enum {
    TWOCC_UNDEF = 0,
     TWOCC_wb,
     TWOCC_db,
     TWOCC_dc,
     TWOCC_pc,
 }AVI_TWOCC_TYPE;
*/

// flags for dwFlags field in AVIMAINHEADER
// prefix AMHF denoting Avi Main Header Flag
#define    AMHF_HASINDEX        0x00000010
#define    AMHF_MUSTUSEINDEX    0x00000020
#define    AMHF_ISINTERLEAVED   0x00000100
#define    AMHF_TRUSTCKTYPE    	0x00000800 /* Use CKType to find key frames? */
#define    AMHF_WASCAPTUREFILE  0x00010000
#define    AMHF_COPYRIGHTED    	0x00020000

typedef struct _avimainheader {
	FOURCC	fcc;
	DWORD   cb;
	DWORD   dwMicroSecPerFrame;
	DWORD   dwMaxBytesPerSec;
	DWORD   dwPaddingGranularity;
	DWORD   dwFlags;
	DWORD   dwTotalFrames;
	DWORD   dwInitialFrames;
	DWORD   dwStreams;
	DWORD   dwSuggestedBufferSize;
	DWORD   dwWidth;
	DWORD   dwHeight;
	DWORD   dwReserved[4];
}AVIMAINHEADER;

typedef struct _avistreamheader {
 	FOURCC    fcc;
 	DWORD     cb;
 	FOURCC    fccType;
 	FOURCC    fccHandler;
 	DWORD     dwFlags;
 	WORD      wPriority;
 	WORD      wLanguage;
 	DWORD      dwInitialFrames;
 	DWORD      dwScale;
 	DWORD      dwRate;
 	DWORD      dwStart;
 	DWORD     dwLength;
 	DWORD     dwSuggestedBufferSize;
	DWORD     dwQuality;
	DWORD     dwSampleSize;

 	struct {
   	 short int left;
   	 short int top;
   	 short int right;
 		short int bottom;
	}rcFrame;

}AVISTREAMHEADER;

// flags for dwFlags field of AVIIDX1ENTRY
// prefix AIEF denoting Avi Index Entry Flag

#define    AIEF_LIST            0x00000001L        // indexed chunk is a list
#define    AIEF_KEYFRAME        0x00000010L        // indexed chunk is a key frame
#define    AIEF_NOTIME            0x00000100L        // indexed chunk frame do not consume any time
#define    AIEF_COMPUSE        0x0FFF0000L        // these bits are used by compressor
#define    AIEF_FIXKEYFRAME    0x00001000L        // XXX: borrowed from VLC, avoid using

typedef struct _aviidx1_entry {
	FOURCC     dwChunkId;
	DWORD     dwFlags;
	DWORD     dwOffset;
	DWORD     dwSize;
}AVIIDX1ENTRY;

typedef struct _avisuperindex_entry {
	DWORDLONG     qwOffset;     // absolute file offset
	DWORD         dwSize;     // size of index chunk at this offset
	DWORD         dwDuration;     // time span in stream ticks
} avisuperindex_entry;

typedef struct _avistdindex_entry {
	DWORD         dwOffset;     // qwBaseOffset + this is absolute file offset
	DWORD         dwSize;     // bit 31 is set if this is NOT a keyframe
} avistdindex_entry;

typedef struct _avistdindex_chunk {
	FOURCC      fcc;                 // ix##
	DWORD       dwSize;         // size of this chunk
	WORD        wLongsPerEntry;     // must be sizeof(aIndex[0])/sizeof(DWORD)
	char        bIndexSubType;     // must be 0
	char        bIndexType;     // must be AVI_INDEX_OF_CHUNKS
	DWORD       nEntriesInUse;         // first unused entry
	FOURCC      dwChunkId;                 // '##dc' or '##db' or '##wb' etc..
	DWORDLONG	qwBaseOffset;     // all dwOffsets in aIndex array are relative to this
	DWORD       dwReserved3;         // must be 0
	avistdindex_entry *aIndex;         // the actual frames
} avistdindex_chunk;

typedef struct _avisuperindex {
	FOURCC 	fcc;         // 'indx'
	DWORD   cb;         // size of this structure
	WORD    wLongsPerEntry;         // ==4
	char    bIndexSubType;         // ==0 (frame index) or AVI_INDEX_SUB_2FIELD
	char    bIndexType;         // ==AVI_INDEX_OF_INDEXES
	DWORD   nEntriesInUse;         // offset of next unused entry in aIndex
	DWORD   dwChunkId;         // chunk ID of chunks being indexed, (i.e. RGB8)
	DWORD   dwReserved[3];         // must be 0
	avisuperindex_entry *aIndex;            // index to std indices
	avistdindex_chunk *stdidx;     // the actual std indices
} AVISUPERINDEX;

 /*********************************customarized defines*************************************/
typedef struct _aviidx1_table {
 	AVIIDX1ENTRY    * idx1_head;
 	AVIIDX1ENTRY    * current_vid_idx;    // for traversing
 	AVIIDX1ENTRY    * current_aud_idx;
 	int             idx1_count;
 	int             idx1_length;
}AVIIDX1TABLE;

typedef struct _aviindex_table {
 	AVISUPERINDEX 	*index_head;
 	int            index_count;
}AVIINDXTABLE;

typedef struct _aviheader {
	// structure for maintaining and accessing
	AVIMAINHEADER    	mainheader;
	AVISTREAMHEADER    	vid_streamheader;
	AVISTREAMHEADER    	aud_streamheader;
	AVIIDX1TABLE    	idx1_table;
	AVIINDXTABLE     	index_table;
	int                	movi_offset;
	int                	movi_length;
	// some indicator
	int             	have_hdrl;
	int             	have_strl;
	int             	have_movi;
	int             	have_idx1;
	int             	have_rec;
	int             	have_indx;
	int             	isodml;        // is OpenDML extension
}AVIHEADER;

typedef struct _aviprop {
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD dwDuration;
	DWORD dwSampleRate;
	WORD  dwNumberOfChannels;
	WORD  dwScanType;
 	DWORD dwAudioBitRate;
	DWORD dwVideoFourCCCodec ;
 	DWORD dwVideoBitRate;
}AVIPROP;

/**************************** AVI object ***************************/
typedef struct _avi {
 	char    	*file_name;    // avi file's name
 	int         fd;                // avi file's descriptor 
 	AVIHEADER   aviheader;
}AVI;

/***************************** real interface ***************************/
//int avi_parse(char * file,AVIPROP *oAVIProp);
int avi_parse(FW_CHAR * file,AVIPROP *oAVIProp);

int avi_init(void);
/************ constructor & deconstructor ******************/
AVI* avi_new(char * file);
void avi_del(void);

#ifdef __cplusplus
 };
#endif    // __cplusplus


#endif    //_LIB_AVI_H_
