/**
 * @file		still_debug.h
 * @brief		capture operator
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _STILL_DEBUG_H_
#define _STILL_DEBUG_H_

#include "ddim_typedef.h"
#include "still_img_info.h"
#include "im_jpeg.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
//#define	_D_LOG_MEDIA_SAVE_

#define D_STILL_DEBUG_RAW_DUMP_FORMAT_12BITPACK					(1)
#define D_STILL_DEBUG_RAW_DUMP_FORMAT_16BIT						(2)

#define D_STILL_DEBUG_RAW_DUMP_OUT_TIMING_SRO					(1)
#define D_STILL_DEBUG_RAW_DUMP_OUT_TIMING_PRO					(2)

#define D_STILL_DEBUG_RAW_DUMP_OUT_SEL_PGAIN0					(1)
#define D_STILL_DEBUG_RAW_DUMP_OUT_SEL_PGAIN2					(2)
#define D_STILL_DEBUG_RAW_DUMP_OUT_SEL_CAG						(3)

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
typedef enum {
	E_STILL_DEBUG_NONE = 0,
	E_STILL_DEBUG_READY,
	E_STILL_DEBUG_START,
	E_STILL_DEBUG_DUMP,
	E_STILL_DEBUG_FILE_SAVE
} E_STILL_DEBUG;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct _RawDumpInfo {
	UCHAR bPRODumpFlag;				// for dumping PRO output image using monitor command.
	UCHAR bR2Y_CNRDumpFlag;			// for dumping R2Y_CNR output image using monitor command.
	UCHAR bSPRDumpFlag;				// for dumping SPR output image using monitor command.
	UCHAR bJPEG_R2Y_CNRDumpFlag;	// for dumping JPEG_R2Y_CNR output image using monitor command.
	UCHAR bJPEG_SPRDumpFlag;		// for dumping JPEG_SPR output image using monitor command.
	UCHAR bJPEG_3DNRDumpFlag;		// for dumping JPEG_3DNR output image using monitor command.
	UCHAR nDumpCount;
	UCHAR nRawDumpFormat;
	UCHAR nRawDumpTiming;
	UCHAR nRawDumpPchSel;
	UCHAR sro_demosaic_id;
	ULLONG dump_frame_no;
	UCHAR dump_enable;
} T_STILL_DEBUG_RAW_DUMP_INFO;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
extern volatile T_STILL_DEBUG_RAW_DUMP_INFO gRawDumpInfo;

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
#define D_STILL_DEBUG_RAW_DUMP_SET_DUMP_COUNT(val)				(gRawDumpInfo.nDumpCount = (val))
#define D_STILL_DEBUG_RAW_DUMP_SET_FORMAT(val)					(gRawDumpInfo.nRawDumpFormat = (val))
#define D_STILL_DEBUG_RAW_DUMP_SET_TIMING(val)					(gRawDumpInfo.nRawDumpTiming = (val))
#define D_STILL_DEBUG_RAW_DUMP_SET_PCHSEL(val)					(gRawDumpInfo.nRawDumpPchSel = (val))
#define D_STILL_DEBUG_RAW_DUMP_SET_PRO_DUMP(val)				(gRawDumpInfo.bPRODumpFlag = (val))
#define D_STILL_DEBUG_RAW_DUMP_SET_R2Y_CNR_DUMP(val)			(gRawDumpInfo.bR2Y_CNRDumpFlag = (val))
#define D_STILL_DEBUG_RAW_DUMP_SET_SPR_DUMP(val)				(gRawDumpInfo.bSPRDumpFlag = (val))
#define D_STILL_DEBUG_RAW_DUMP_SET_JPEG_R2Y_CNR_DUMP(val)		(gRawDumpInfo.bJPEG_R2Y_CNRDumpFlag = (val))
#define D_STILL_DEBUG_RAW_DUMP_SET_JPEG_SPR_DUMP(val)			(gRawDumpInfo.bJPEG_SPRDumpFlag = (val))
#define D_STILL_DEBUG_RAW_DUMP_SET_JPEG_3DNR_DUMP(val)			(gRawDumpInfo.bJPEG_3DNRDumpFlag = (val))
#define D_STILL_DEBUG_RAW_DUMP_SET_SRO_DEMOSAIC_ID(val)			(gRawDumpInfo.sro_demosaic_id = (val))
#define D_STILL_DEBUG_RAW_DUMP_SET_FRAME_NO(val)				(gRawDumpInfo.dump_frame_no = (val))
#define D_STILL_DEBUG_RAW_DUMP_SET_ENABLE(val)					(gRawDumpInfo.dump_enable = (val))

#define D_STILL_DEBUG_RAW_DUMP_GET_DUMP_COUNT()					(gRawDumpInfo.nDumpCount)
#define D_STILL_DEBUG_RAW_DUMP_GET_FORMAT()						(gRawDumpInfo.nRawDumpFormat)
#define D_STILL_DEBUG_RAW_DUMP_GET_TIMING()						(gRawDumpInfo.nRawDumpTiming)
#define D_STILL_DEBUG_RAW_DUMP_GET_PCHSEL()						(gRawDumpInfo.nRawDumpPchSel)
#define D_STILL_DEBUG_RAW_DUMP_IS_PRO_DUMP()					(gRawDumpInfo.bPRODumpFlag)
#define D_STILL_DEBUG_RAW_DUMP_IS_R2Y_CNR_DUMP()				(gRawDumpInfo.bR2Y_CNRDumpFlag)
#define D_STILL_DEBUG_RAW_DUMP_IS_SPR_DUMP()					(gRawDumpInfo.bSPRDumpFlag)
#define D_STILL_DEBUG_RAW_DUMP_IS_JPEG_R2Y_CNR_DUMP()			(gRawDumpInfo.bJPEG_R2Y_CNRDumpFlag)
#define D_STILL_DEBUG_RAW_DUMP_IS_JPEG_SPR_DUMP()				(gRawDumpInfo.bJPEG_SPRDumpFlag)
#define D_STILL_DEBUG_RAW_DUMP_IS_JPEG_3DNR_DUMP()				(gRawDumpInfo.bJPEG_3DNRDumpFlag)
#define D_STILL_DEBUG_RAW_DUMP_GET_SRO_DEMOSAIC_ID()			(gRawDumpInfo.sro_demosaic_id)
#define D_STILL_DEBUG_RAW_DUMP_GET_FRAME_NO()					(gRawDumpInfo.dump_frame_no)
#define D_STILL_DEBUG_RAW_DUMP_GET_ENABLE()						(gRawDumpInfo.dump_enable)

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
#ifdef _D_LOG_MEDIA_SAVE_
extern VOID BF_Still_Debug_Print( CHAR* data );
#endif
extern VOID BF_Still_Debug_Set_View_Raw_Save_On( E_STILL_DEBUG status );
extern VOID BF_Still_Debug_Set_View_Raw_Save_Off( VOID );
extern E_STILL_DEBUG BF_Still_Debug_Is_View_Raw_Save_On( VOID );
extern UCHAR BF_Still_Debug_Is_Still_Raw_Save_On( VOID );
extern UCHAR BF_Still_Debug_Is_Still_Raw_Save_Type( VOID );
extern VOID BF_Still_Debug_Set_Still_Raw_Save_On( UCHAR bayer_type );
extern VOID BF_Still_Debug_Set_Still_Raw_Save_Off( VOID );
extern VOID BF_Still_Debug_Check_Raw_Next_File( USHORT* next_dirno, USHORT* next_fileno );
extern VOID BF_Still_Debug_Raw_Save( ULONG bay_addr, UCHAR fileno_diff );
extern VOID BF_Still_Debug_Dump_Break_Func( VOID );
extern VOID BF_Still_Debug_Save( char* filename, char* filetype, ULONG ycc_addr, ULONG size );
extern VOID BF_Still_Debug_Error( ULONG error_factor, USHORT channel );


#endif	// _STILL_DEBUG_H_
