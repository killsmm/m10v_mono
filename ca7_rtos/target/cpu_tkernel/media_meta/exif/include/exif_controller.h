/**
 * @file		exif_controller.h
 * @brief		create DCF Exif tag format(header).
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef __DCF_EXIF
#define __DCF_EXIF

#include "fj_exif.h"
#include "fs_user_custom.h"

#include "dcf_if.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/* Exif Type */
typedef FW_UCHAR	DCF_BYTE;
typedef FW_CHAR		DCF_ASCII;
typedef FW_USHORT	DCF_SHORT;
typedef FW_ULONG	DCF_LONG;
typedef FW_UCHAR	DCF_UNDEFINED;
typedef FW_INT32	DCF_SLONG;

#define D_BF_DCF_EXIF_BYTE_T				(1)
#define D_BF_DCF_EXIF_ASCII_T				(2)
#define D_BF_DCF_EXIF_SHORT_T				(3)
#define D_BF_DCF_EXIF_LONG_T				(4)
#define D_BF_DCF_EXIF_RATIONAL_T			(5)
#define D_BF_DCF_EXIF_UNDEFINED_T			(7)
#define D_BF_DCF_EXIF_SLONG_T				(9)
#define D_BF_DCF_EXIF_SRATIONAL_T			(10)

#define D_BF_DCF_EXIF_BIG_EN				(0x4D4D)
#define D_BF_DCF_EXIF_LIT_EN				(0x4949)
#define D_BF_DCF_EXIF_TIFF_HEARD_RESERVED	(0x002A)
#define D_BF_DCF_EXIF_TIFF_HEARD_OFFSET		(0x00000008)

/* 0th IFD size */
#define D_BF_DCF_EXIF_V_DESCRIPTION_S		FJ_CUSTOM_EXIF_IMG_DESCRIPTION_S	/* 4 align */
#define D_BF_DCF_EXIF_V_MAKE_S				FJ_CUSTOM_EXIF_MAKE_S
#define D_BF_DCF_EXIF_V_MODEL_S				FJ_CUSTOM_EXIF_MODEL_S				/* 4 align */
#define D_BF_DCF_EXIF_V_SOFT_S				FJ_CUSTOM_EXIF_SOFT_S				/* 4 align */
#define D_BF_DCF_EXIF_V_DATETIME_S			(20)								/* fixed */
#define D_BF_DCF_EXIF_V_COPYRIGHT_S			FJ_CUSTOM_EXIF_COPYRIGHT_S			/* 4 align */
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
#define D_BF_DCF_EXIF_V_PRINTIM_S			FJ_CUSTOM_EXIF_PRINTIM_S			/* 4 align */
#endif

/* Exif IFD size */
#define D_BF_DCF_EXIF_V_DATEORIG_S 			(20)								/* fixed */
#define D_BF_DCF_EXIF_V_DATEDIGI_S 			(20)								/* fixed */

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
/* 1th IFD size */
#define D_BF_DCF_EXIF_V_MAKER1_S			FJ_CUSTOM_EXIF_MAKER1_S
#define D_BF_DCF_EXIF_V_MODEL1_S			FJ_CUSTOM_EXIF_MODEL1_S				/* 4 align */
#define D_BF_DCF_EXIF_V_DATETIME1_S			(20)								/* fixed */
#endif

#define D_BF_DCF_EXIF_TIFFHEADSIZE			(sizeof(T_BF_DCF_EXIF_TIFF_HEADER))
#define D_BF_DCF_EXIF_TIFFOFFSET			(0xC)

#define D_BF_DCF_EXIF_T_IFD_SIZE			(sizeof(T_BF_DCF_EXIF_IFD))

#define D_BF_DCF_EXIF_JPEG_ZERO_VALUE_S		(sizeof(T_BF_DCF_EXIF_JPEG_ZERO_VALUE))
#define D_BF_DCF_EXIF_JPEG_EXIF_VALUE_S		(sizeof(T_BF_DCF_EXIF_JPEG_EXIF_VALUE))
#define D_BF_DCF_EXIF_JPEG_FIRST_VALUE_S	(sizeof(T_BF_DCF_EXIF_JPEG_FIRST_VALUE))


/**************************************************************************/
/* ---------------- Definition for JPEG file ---------------------------- */
/**************************************************************************/
/* Exif Tag Number */
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
#ifdef CO_BF_DCF_IF_SET_GPSTAG
#define D_BF_DCF_EXIF_IFD_NUM_OF_IFD0			(13+1)								/* sum of IFD in 0th IFD */ //+1gps
#else
#define D_BF_DCF_EXIF_IFD_NUM_OF_IFD0			(13)								/* sum of IFD in 0th IFD */
#endif
#define D_BF_DCF_EXIF_IFD_NUM_OF_IFDEXIF		(35)								/* sum of IFD in Exif IFD */
#define D_BF_DCF_EXIF_IFD_NUM_OF_R98			(2)									/* sum of IFD in R98 IFD */
#define D_BF_DCF_EXIF_IFD_NUM_OF_IFD1			(10)								/* sum of IFD in 1st IFD */
#else
#ifdef CO_BF_DCF_IF_SET_GPSTAG
#define D_BF_DCF_EXIF_IFD_NUM_OF_IFD0			(12+1)								/* sum of IFD in 0th IFD */ //+1gps
#else
#define D_BF_DCF_EXIF_IFD_NUM_OF_IFD0			(12)								/* sum of IFD in 0th IFD */
#endif
#define D_BF_DCF_EXIF_IFD_NUM_OF_IFDEXIF		(32)								/* sum of IFD in Exif IFD */
#define D_BF_DCF_EXIF_IFD_NUM_OF_R98			(2)									/* sum of IFD in R98 IFD */
#define D_BF_DCF_EXIF_IFD_NUM_OF_IFD1			(7)									/* sum of IFD in 1st IFD */
#endif
#ifdef CO_BF_DCF_IF_SET_GPSTAG
#define D_BF_DCF_EXIF_IFD_NUM_OF_GPS			(6)									/* sum of IFD in GPS IFD */
#endif

/* 0th IFD offset */
#define D_BF_DCF_EXIF_O_DESCRIPTION				(D_BF_DCF_EXIF_TIFFHEADSIZE + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * D_BF_DCF_EXIF_IFD_NUM_OF_IFD0) + 4)
#define D_BF_DCF_EXIF_O_MAKE					(D_BF_DCF_EXIF_O_DESCRIPTION + D_BF_DCF_EXIF_V_DESCRIPTION_S)
#define D_BF_DCF_EXIF_O_MODEL					(D_BF_DCF_EXIF_O_MAKE        + D_BF_DCF_EXIF_V_MAKE_S)
#define D_BF_DCF_EXIF_O_XRESOL					(D_BF_DCF_EXIF_O_MODEL       + D_BF_DCF_EXIF_V_MODEL_S)
#define D_BF_DCF_EXIF_O_YRESOL					(D_BF_DCF_EXIF_O_XRESOL      + D_BF_DCF_EXIF_RATIONAL_S)
#define D_BF_DCF_EXIF_O_SOFT					(D_BF_DCF_EXIF_O_YRESOL      + D_BF_DCF_EXIF_RATIONAL_S)
#define D_BF_DCF_EXIF_O_DATETIME				(D_BF_DCF_EXIF_O_SOFT        + D_BF_DCF_EXIF_V_SOFT_S)
#define D_BF_DCF_EXIF_O_COPYRIGHT				(D_BF_DCF_EXIF_O_DATETIME    + D_BF_DCF_EXIF_V_DATETIME_S)
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
#define D_BF_DCF_EXIF_O_PRINTIM					(D_BF_DCF_EXIF_O_COPYRIGHT   + D_BF_DCF_EXIF_V_COPYRIGHT_S)
#endif
#define D_BF_DCF_EXIF_O_EXIFPOINTER				(D_BF_DCF_EXIF_TIFFHEADSIZE + 2 + sizeof(T_BF_DCF_EXIF_JPEG_ZERO_IFD) + sizeof(T_BF_DCF_EXIF_JPEG_ZERO_VALUE))

/* Exif IFD offset */
#define D_BF_DCF_EXIF_O_EXPTIME					(D_BF_DCF_EXIF_O_EXIFPOINTER + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * D_BF_DCF_EXIF_IFD_NUM_OF_IFDEXIF) + 4)
#define D_BF_DCF_EXIF_O_FNUMBER					(D_BF_DCF_EXIF_O_EXPTIME      + D_BF_DCF_EXIF_RATIONAL_S)
#define D_BF_DCF_EXIF_O_DATEORIG				(D_BF_DCF_EXIF_O_FNUMBER      + D_BF_DCF_EXIF_RATIONAL_S)
#define D_BF_DCF_EXIF_O_DATEDIGI				(D_BF_DCF_EXIF_O_DATEORIG     + D_BF_DCF_EXIF_V_DATEORIG_S)
#define D_BF_DCF_EXIF_O_BITSPERPIXEL			(D_BF_DCF_EXIF_O_DATEDIGI     + D_BF_DCF_EXIF_V_DATEDIGI_S)
#define D_BF_DCF_EXIF_O_SHUTTER					(D_BF_DCF_EXIF_O_BITSPERPIXEL + D_BF_DCF_EXIF_RATIONAL_S)
#define D_BF_DCF_EXIF_O_APERTUE					(D_BF_DCF_EXIF_O_SHUTTER      + D_BF_DCF_EXIF_SRATIONAL_S)
#define D_BF_DCF_EXIF_O_EXPOSUREBIAS			(D_BF_DCF_EXIF_O_APERTUE      + D_BF_DCF_EXIF_RATIONAL_S)
#define D_BF_DCF_EXIF_O_MAXAPERTUER				(D_BF_DCF_EXIF_O_EXPOSUREBIAS + D_BF_DCF_EXIF_SRATIONAL_S)
#define D_BF_DCF_EXIF_O_FOCAL					(D_BF_DCF_EXIF_O_MAXAPERTUER  + D_BF_DCF_EXIF_RATIONAL_S)
#define D_BF_DCF_EXIF_O_DIGIZOOM				(D_BF_DCF_EXIF_O_FOCAL        + D_BF_DCF_EXIF_RATIONAL_S)
#define D_BF_DCF_EXIF_O_MAKERNOTE				(D_BF_DCF_EXIF_O_DIGIZOOM     + D_BF_DCF_EXIF_RATIONAL_S)

#define D_BF_DCF_EXIF_O_R98POINTER				(D_BF_DCF_EXIF_O_EXIFPOINTER + 2 + sizeof(T_BF_DCF_EXIF_JPEG_EXIF_IFD) + sizeof(T_BF_DCF_EXIF_JPEG_EXIF_VALUE))

#ifdef CO_BF_DCF_IF_SET_GPSTAG
#define D_BF_DCF_EXIF_GPS_IFD_POINTER			(D_BF_DCF_EXIF_O_R98POINTER + 2 + sizeof(T_BF_DCF_EXIF_JPEG_R98_IFD))

/* Gps IFD offset */
#define D_BF_DCF_EXIF_O_GPSLATITUDE				(D_BF_DCF_EXIF_GPS_IFD_POINTER + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * D_BF_DCF_EXIF_IFD_NUM_OF_GPS) + 4)
#define D_BF_DCF_EXIF_O_GPSLONGITUDE			(D_BF_DCF_EXIF_O_GPSLATITUDE + (sizeof(T_BF_DCF_EXIF_RATIONAL))*3)
#define D_BF_DCF_EXIF_O_GPSIMGDIRECTION			(D_BF_DCF_EXIF_O_GPSLONGITUDE + (sizeof(T_BF_DCF_EXIF_RATIONAL))*3)

#define D_BF_DCF_EXIF_NEXT_IFD_POINTER			(D_BF_DCF_EXIF_GPS_IFD_POINTER + 2 + sizeof(T_BF_DCF_EXIF_JPEG_GPS_IFD) + sizeof(T_BF_DCF_EXIF_JPEG_GPS_VALUE))
#else
/* R98 Exif IFD offset */
#define D_BF_DCF_EXIF_NEXT_IFD_POINTER			(D_BF_DCF_EXIF_O_R98POINTER + 2 + sizeof(T_BF_DCF_EXIF_JPEG_R98_IFD))
#endif

/* 1st IFD offset */
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
#define D_BF_DCF_EXIF_O_MAKER1					(D_BF_DCF_EXIF_NEXT_IFD_POINTER + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * D_BF_DCF_EXIF_IFD_NUM_OF_IFD1) + 4)
#define D_BF_DCF_EXIF_O_MODEL1					(D_BF_DCF_EXIF_O_MAKER1 + D_BF_DCF_EXIF_V_MAKER1_S)
#define D_BF_DCF_EXIF_O_XRES_1ST				(D_BF_DCF_EXIF_O_MODEL1 + D_BF_DCF_EXIF_V_MODEL1_S)
#else
#define D_BF_DCF_EXIF_O_XRES_1ST				(D_BF_DCF_EXIF_NEXT_IFD_POINTER + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * D_BF_DCF_EXIF_IFD_NUM_OF_IFD1) + 4)
#endif
#define D_BF_DCF_EXIF_O_YRES_1ST				(D_BF_DCF_EXIF_O_XRES_1ST  + D_BF_DCF_EXIF_RATIONAL_S)
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
#define D_BF_DCF_EXIF_O_DATETIME1				(D_BF_DCF_EXIF_O_YRES_1ST  + D_BF_DCF_EXIF_RATIONAL_S)
#define D_BF_DCF_EXIF_THUMBNAIL_DATA_OFFSET		(D_BF_DCF_EXIF_O_DATETIME1 + D_BF_DCF_EXIF_V_DATETIME1_S)
#else
#define D_BF_DCF_EXIF_THUMBNAIL_DATA_OFFSET		(D_BF_DCF_EXIF_O_YRES_1ST  + D_BF_DCF_EXIF_RATIONAL_S)
#endif
#define D_BF_DCF_EXIF_THUM_START_ADD			(D_BF_DCF_EXIF_THUMBNAIL_DATA_OFFSET + D_BF_DCF_EXIF_TIFFOFFSET)	/* thumbnail image stored in this address */ 

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
// For CPU big-endian
#define D_BF_DCF_EXIF_SOI						(0xFFD8)
#define D_BF_DCF_EXIF_APP1_MARKER				(0xFFE1)
#define D_BF_DCF_EXIF_APP2_MARKER				(0xFFE2)
#else
// For CPU little-endian
#define D_BF_DCF_EXIF_SOI						(0xD8FF)
#define D_BF_DCF_EXIF_APP1_MARKER				(0xE1FF)
#define D_BF_DCF_EXIF_APP2_MARKER				(0xE2FF)
#endif
#define D_BF_DCF_EXIF_SOI_SIZE					(4)
#define D_BF_DCF_EXIF_MAX_APP1_SIZE				(65535)


/******************************************************************************/
/* for THM file **************************************************************/
/******************************************************************************/
#define D_BF_DCF_EXIF_IFD_NUM_OF_IFD0_THM		(9)										/* sum of IFD in 0th IFD */
#define D_BF_DCF_EXIF_IFD_NUM_OF_IFDEXIF_THM	(9)										/* sum of IFD in Exif IFD */
#define D_BF_DCF_EXIF_IFD_NUM_OF_R98_THM		(2)										/* sum of IFD in R98 IFD */

/* 0th IFD offset */
#define D_BF_DCF_EXIF_O_MAKE_THM				(D_BF_DCF_EXIF_TIFFHEADSIZE + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * D_BF_DCF_EXIF_IFD_NUM_OF_IFD0_THM) + 4)
#define D_BF_DCF_EXIF_O_MODEL_THM				(D_BF_DCF_EXIF_O_MAKE_THM     + D_BF_DCF_EXIF_V_MAKE_S)
#define D_BF_DCF_EXIF_O_XRESOL_THM				(D_BF_DCF_EXIF_O_MODEL_THM    + D_BF_DCF_EXIF_V_MODEL_S)
#define D_BF_DCF_EXIF_O_YRESOL_THM				(D_BF_DCF_EXIF_O_XRESOL_THM   + D_BF_DCF_EXIF_RATIONAL_S)
#define D_BF_DCF_EXIF_O_DATETIME_THM			(D_BF_DCF_EXIF_O_YRESOL_THM   + D_BF_DCF_EXIF_RATIONAL_S)
#define D_BF_DCF_EXIF_O_EXIFPOINTER_THM			(D_BF_DCF_EXIF_O_DATETIME_THM + D_BF_DCF_EXIF_V_DATETIME_S)

/* Exif IFD offset */
#define D_BF_DCF_EXIF_O_DATEORIG_THM			(D_BF_DCF_EXIF_O_EXIFPOINTER_THM + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * D_BF_DCF_EXIF_IFD_NUM_OF_IFDEXIF_THM) + 4)
#define D_BF_DCF_EXIF_O_DATEDIGI_THM			(D_BF_DCF_EXIF_O_DATEORIG_THM + D_BF_DCF_EXIF_V_DATEORIG_S)

#define D_BF_DCF_EXIF_O_R98POINTER_THM			(D_BF_DCF_EXIF_O_DATEDIGI_THM + D_BF_DCF_EXIF_V_DATEDIGI_S)

#define D_BF_DCF_EXIF_THUM_START_ADD_THM		(D_BF_DCF_EXIF_O_R98POINTER_THM + 2 + sizeof(T_BF_DCF_EXIF_JPEG_R98_IFD))	/* thumbnail image stored in this address */ 
/**************************************************************************************/

#define D_BF_DCF_EXIF_MAKER_SEGMENT_LIMIT_SIZE	(0xFE00)							// Maker segment limit size = 0xFFFF (include segment header)

/* Exif Type size */
#define D_BF_DCF_EXIF_SHORT_S				(sizeof(DCF_SHORT))
#define D_BF_DCF_EXIF_LONG_S				(sizeof(DCF_LONG))
#define D_BF_DCF_EXIF_RATIONAL_S			(sizeof(T_BF_DCF_EXIF_RATIONAL))
#define D_BF_DCF_EXIF_SRATIONAL_S			(sizeof(T_BF_DCF_EXIF_SRATIONAL))
#define D_BF_DCF_EXIF_APP1_MARKER_S			(sizeof(T_BF_DCF_EXIF_APP1_MARKER))
#define D_BF_DCF_EXIF_TIFF_HEADER_S			(sizeof(T_BF_DCF_EXIF_TIFF_HEADER))
#define D_BF_DCF_EXIF_JPEG_ZERO_IFD_S		(sizeof(T_BF_DCF_EXIF_JPEG_ZERO_IFD))
#define D_BF_DCF_EXIF_JPEG_EXIF_IFD_S		(sizeof(T_BF_DCF_EXIF_JPEG_EXIF_IFD))
#define D_BF_DCF_EXIF_JPEG_R98_IFD_S		(sizeof(T_BF_DCF_EXIF_JPEG_R98_IFD))
#define D_BF_DCF_EXIF_JPEG_FIRST_IFD_S		(sizeof(T_BF_DCF_EXIF_JPEG_FIRST_IFD))

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
//-----------------------------------------------------------------------------
// for MPF
//-----------------------------------------------------------------------------
#define T_BF_DCF_EXIF_MP_HEADER				T_BF_DCF_EXIF_TIFF_HEADER
#define D_BF_DCF_EXIF_MP_HEADER_SIZE		D_BF_DCF_EXIF_TIFFHEADSIZE
#define D_BF_DCF_EXIF_MP_HEAD_RESERVED		D_BF_DCF_EXIF_TIFF_HEARD_RESERVED
#define D_BF_DCF_EXIF_MP_HEAD_OFFSET		D_BF_DCF_EXIF_TIFF_HEARD_OFFSET
#define D_BF_DCF_EXIF_MP_INDEX_IFD_NUM		(3)
#define D_BF_DCF_EXIF_MP_ENTRY_OFFSET		(D_BF_DCF_EXIF_MP_HEADER_SIZE + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * D_BF_DCF_EXIF_MP_INDEX_IFD_NUM) + 4)

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
#define D_BF_DCF_EXIF_EXT_MP_ATTRIBUTE_IFD_NUM   (3)
#endif

#define D_BF_DCF_EXIF_IMAGEUID_SIZE			(33)

#define D_BF_DCF_EXIF_MPF_IFD0_NUM			(1)
#define D_BF_DCF_EXIF_MPF_EXIFIFD_NUM		(2)
#define D_BF_DCF_EXIF_MPF_IFD1_NUM			(2)

#define D_BF_DCF_EXIF_MPF_EXIFIFD_OFFSET	(D_BF_DCF_EXIF_TIFFHEADSIZE              + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * D_BF_DCF_EXIF_MPF_IFD0_NUM)    + 4)
#define D_BF_DCF_EXIF_MPF_IFD1_OFFSET		(D_BF_DCF_EXIF_MPF_EXIFIFD_OFFSET + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * D_BF_DCF_EXIF_MPF_EXIFIFD_NUM) + 4)
#define D_BF_DCF_EXIF_MPF_THUMB_OFFSET		(D_BF_DCF_EXIF_MPF_IFD1_OFFSET    + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * D_BF_DCF_EXIF_MPF_IFD1_NUM)    + 4)

#endif // CO_BF_DCF_IF_MPF_ENABLE


/* IFD Default Value Short */
#ifdef FJ_CUSTOM_EXIF_BIG_ENDIAN
// big-endian
#define D_BF_DCF_EXIF_DVAL_ORIENT			0x00010000		/* 0x112   Orientation      */
#define D_BF_DCF_EXIF_DVAL_RESOLUNIT		0x00020000		/* 0x128   ResolutionUnit   */
#define D_BF_DCF_EXIF_DVAL_YCBCRPOS			0x00020000		/* 0x213   YCbCrPositioning */
#define D_BF_DCF_EXIF_DVAL_EXPOSPROG		0x00020000		/* 0x8822  ExposureProgram  */
#define D_BF_DCF_EXIF_DVAL_ISOSPEED			0x00640000		/* 0x8827  ISOSpeedRatings  */
#define D_BF_DCF_EXIF_DVAL_COLORSPACE		0x00010000		/* 0xA001  ColorSpace       */
#define D_BF_DCF_EXIF_DVAL_SENSMETHOD		0x00020000		/* 0xA217  SensingMethod    */
#define D_BF_DCF_EXIF_DVAL_COMPRESSION		0x00060000		/* 0x103   Compression      */
#else
// little-endian
#define D_BF_DCF_EXIF_DVAL_ORIENT			0x00000001		/* 0x112   Orientation      */
#define D_BF_DCF_EXIF_DVAL_RESOLUNIT		0x00000002		/* 0x128   ResolutionUnit   */
#define D_BF_DCF_EXIF_DVAL_YCBCRPOS			0x00000002		/* 0x213   YCbCrPositioning */
#define D_BF_DCF_EXIF_DVAL_EXPOSPROG		0x00000002		/* 0x8822  ExposureProgram  */
#define D_BF_DCF_EXIF_DVAL_ISOSPEED			0x00000064		/* 0x8827  ISOSpeedRatings  */
#define D_BF_DCF_EXIF_DVAL_COLORSPACE		0x00000001		/* 0xA001  ColorSpace       */
#define D_BF_DCF_EXIF_DVAL_SENSMETHOD		0x00000002		/* 0xA217  SensingMethod    */
#define D_BF_DCF_EXIF_DVAL_COMPRESSION		0x00000006		/* 0x103   Compression      */
#endif

/* IFD Default Value Undefined */
#ifdef FJ_CUSTOM_EXIF_BIG_ENDIAN
// big-endian
#define D_BF_DCF_EXIF_DVAL_EXIFVER			0x30323231		/* 0x9000   ExifVersion             */ /* "0221"      */
#define D_BF_DCF_EXIF_DVAL_COMPONENTCONFIG	0x01020300		/* 0x9101   ComponentsConfiguration */ /* 1230(YCbCr) */
#define D_BF_DCF_EXIF_DVAL_FLASHPIXVER		0x30313030		/* 0xA000   FlashPixVersion         */ /* "0100"      */
#define D_BF_DCF_EXIF_DVAL_FILESOURCE		0x03000000		/* 0xA300   FileSource              */ /* 3:DSC       */
#define D_BF_DCF_EXIF_DVAL_SCENETYPE		0x01000000		/* 0xA301   SceneType               */ /* 1:A directly photographed image */
#define D_BF_DCF_EXIF_DVAL_INTEROPEVER		0x30313030		/* 0x2      InteroperabilityVersion */ /* "0100"      */
#define D_BF_DCF_EXIF_DVAL_MPFVER			0x30313030		/* 0xB000   MP format Version       */ /* "0100"      */
#else
// little-endian
#define D_BF_DCF_EXIF_DVAL_EXIFVER			0x31323230		/* 0x9000   ExifVersion             */ /* "0221"      */
#define D_BF_DCF_EXIF_DVAL_COMPONENTCONFIG	0x00030201		/* 0x9101   ComponentsConfiguration */ /* 1230(YCbCr) */
#define D_BF_DCF_EXIF_DVAL_FLASHPIXVER		0x30303130		/* 0xA000   FlashPixVersion         */ /* "0100"      */
#define D_BF_DCF_EXIF_DVAL_FILESOURCE		0x00000003		/* 0xA300   FileSource              */ /* 3:DSC       */
#define D_BF_DCF_EXIF_DVAL_SCENETYPE		0x00000001		/* 0xA301   SceneType               */ /* 1:A directly photographed image */
#define D_BF_DCF_EXIF_DVAL_INTEROPEVER		0x30303130		/* 0x2      InteroperabilityVersion */ /* "0100"      */
#define D_BF_DCF_EXIF_DVAL_MPFVER			0x30303130		/* 0xB000   MP format Version       */ /* "0100"      */
#endif

/* IFD Default Value Ascii */
#ifdef FJ_CUSTOM_EXIF_BIG_ENDIAN
// big-endian
#define D_BF_DCF_EXIF_DVAL_INTEROPEINDEX		0x52393800	/* 0x1      InteroperabilityIndex   */ /* "R98"       */
#define D_BF_DCF_EXIF_DVAL_INTEROPEINDEX_THM	0x54484D00	/* 0x1      InteroperabilityIndex   */ /* "THM"       */
#else
// little-endian
#define D_BF_DCF_EXIF_DVAL_INTEROPEINDEX		0x00383952	/* 0x1      InteroperabilityIndex   */ /* "R98"       */
#define D_BF_DCF_EXIF_DVAL_INTEROPEINDEX_THM	0x004D4854	/* 0x1      InteroperabilityIndex   */ /* "THM"       */
#endif

#ifdef CO_BF_DCF_IF_SET_GPSTAG
/* GPS Default Value Ascii */
#ifdef FJ_CUSTOM_EXIF_BIG_ENDIAN
// big-endian
#define D_BF_DCF_EXIF_DVAL_GPSLATITUDEREF	0x4E530000	/* 0x1      GPSLatitudeRef     */ /* "NS"       */
#define D_BF_DCF_EXIF_DVAL_GPSLONGITUDEREF	0x45570000	/* 0x1      GPSLongitudeRef    */ /* "EW"       */
#define D_BF_DCF_EXIF_DVAL_GPSIMGDIRECTION	0x544d0000	/* 0x1      GPSImgDirectionRef */ /* "TM"       */
#else
// little-endian
#define D_BF_DCF_EXIF_DVAL_GPSLATITUDEREF	0x0000534E	/* 0x1      GPSLatitudeRef     */ /* "NS"       */
#define D_BF_DCF_EXIF_DVAL_GPSLONGITUDEREF	0x00005745	/* 0x1      GPSLongitudeRef    */ /* "EW"       */
#define D_BF_DCF_EXIF_DVAL_GPSIMGDIRECTION	0x00004d54	/* 0x1      GPSImgDirectionRef */ /* "TM"       */
#endif
#endif

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/**
 * @breif RATIONAL structure for DCF
 * @note none
 * @attention none
 */
typedef struct {
	DCF_LONG Num;	/* Numerator */
	DCF_LONG Den;	/* Denominator */
} T_BF_DCF_EXIF_RATIONAL;

/**
 * @breif SRATIONAL structure for DCF
 * @note none
 * @attention none
 */
typedef struct {
	DCF_SLONG Num;	/* Numerator */
	DCF_SLONG Den;	/* Denominator */
} T_BF_DCF_EXIF_SRATIONAL;


/**
 * @breif TIFF header
 * @note none
 * @attention none
 */
typedef struct {
	DCF_SHORT ByteOrder;
	DCF_SHORT Reserved;
	DCF_LONG  Offset;
} T_BF_DCF_EXIF_TIFF_HEADER;


/**
 * @breif SHORT or LONG structure
 * @note none
 * @attention none
 */
typedef struct {
	DCF_SHORT Value;
	DCF_SHORT dummy;			/* no use */
} T_BF_DCF_EXIF_SHORT_ON_LONG;

/**
 * @breif IFD structure
 * @note none
 * @attention none
 */
typedef union {
	DCF_LONG        Offset;
	DCF_LONG        LValue;
	T_BF_DCF_EXIF_SHORT_ON_LONG SValue;
	DCF_UNDEFINED   UValue[4];
} U_BF_DCF_EXIF_VALUE_OR_OFFSET;

/**
 * @breif IFD structure
 * @note none
 * @attention none
 */
typedef struct {
	DCF_SHORT         Tag;
	DCF_SHORT         Type;
	DCF_LONG          Count;
	U_BF_DCF_EXIF_VALUE_OR_OFFSET ValueOffset;
} T_BF_DCF_EXIF_IFD;


/**
 * @breif APP1 marker
 * @note none
 * @attention none
 */
typedef struct {
	DCF_SHORT App1Marker;
	DCF_SHORT FieldLength;
	DCF_ASCII ExifIdCode[5];
	DCF_BYTE  Padding;
} T_BF_DCF_EXIF_APP1_MARKER;

#if 0
/**
 * @breif 0th IFD of Exif
 * @note none
 * @attention none
 */
typedef struct {
	/*  Field Name          Type(Count)     Tag Name                  */
	/*                                      M:Mandatory               */
	/*                                      R:Conditionally mandatory */
	/*                                      O:Optional                */
	T_BF_DCF_EXIF_IFD ImageDescription ;	/* ASCII(Any)	R	Image title */
	T_BF_DCF_EXIF_IFD Make ;				/* ASCII(Any)	R	Image input equipment manufacturer */
	T_BF_DCF_EXIF_IFD Model ;				/* ASCII(Any)	R	Image input equipment model */
	T_BF_DCF_EXIF_IFD Orientation ;			/* SHORT(1)	R	Orientation of image */
	T_BF_DCF_EXIF_IFD XResolution ;			/* RATIONAL(1)	M	Image resolution in width direction */
	T_BF_DCF_EXIF_IFD YResolution ;			/* RATIONAL(1)	M	Image resolution in height direction */
	T_BF_DCF_EXIF_IFD ResolutionUnit ;		/* SHORT(1)	M	Unit of X and Y resolution */
	T_BF_DCF_EXIF_IFD Software ;			/* ASCII(Any)	O	Software used */
	T_BF_DCF_EXIF_IFD DateTime ;			/* ASCII(20)	R	File change date and time */
	T_BF_DCF_EXIF_IFD YCbCrPositioning ;	/* SHORT(1)	M	Y and C positioning */
	T_BF_DCF_EXIF_IFD Copyright ;			/* ASCII(Any)	O	Copyright holder */
	T_BF_DCF_EXIF_IFD ExifIFDPointer ;		/* LONG(1)	M	Exif Tag */

	FW_ULONG offset_to_1stIFD ;/* offset to 1st IFD */

	DCF_ASCII    ValueImageDescription[D_BF_DCF_EXIF_V_DESCRIPTION_S] ;
	DCF_ASCII    ValueMake[D_BF_DCF_EXIF_V_MAKE_S] ;
	DCF_ASCII    ValueModel[D_BF_DCF_EXIF_V_MODEL_S] ;
	T_BF_DCF_EXIF_RATIONAL ValueXResolution ;
	T_BF_DCF_EXIF_RATIONAL ValueYResolution ;
	DCF_ASCII    ValueSoftware[D_BF_DCF_EXIF_V_SOFT_S] ;
	DCF_ASCII    ValueDateTime[D_BF_DCF_EXIF_V_DATETIME_S] ;
	DCF_ASCII    ValueCopyright[D_BF_DCF_EXIF_V_COPYRIGHT_S];
} T_BF_DCF_EXIF_JPEG_ZERO_IFD ;

/**
 * @breif Exif IFD of Exif
 * @note none
 * @attention none
 */
typedef struct {
	T_BF_DCF_EXIF_IFD ExposureTime ;				/* RATIONAL(1)	R	Exposure time */
	T_BF_DCF_EXIF_IFD FNumber ;						/* RATIONAL(1)	O	F number */
	T_BF_DCF_EXIF_IFD ExposureProgram ;				/* SHORT(1)	O	Exposure program */
	T_BF_DCF_EXIF_IFD ISOSpeedRatings ;				/* SHORT(1)	O	ISO speed ratings */
	T_BF_DCF_EXIF_IFD ExifVersion ;					/* UNDEFINED(4)	M	Exif version */
	T_BF_DCF_EXIF_IFD DateTimeOriginal ;			/* ASCII(20)	O	Date and time of original data generation */
	T_BF_DCF_EXIF_IFD DateTimeDigitized ;			/* ASCII(20)	O	Date and time of digital data generation */
	T_BF_DCF_EXIF_IFD ComponentsConfiguration ;		/* UNDEFINED(4)	M	Meaning of each component */
	T_BF_DCF_EXIF_IFD CompressedBitsPerPixel ;		/* RATIONAL(1)	O	Image compression mode */
	T_BF_DCF_EXIF_IFD ShutterSpeedValue ;			/* SRATIONAL(1) 	O	Shutter speed */
	T_BF_DCF_EXIF_IFD ApertureValue ;				/* RATIONAL(1)	O	Aperture */
	T_BF_DCF_EXIF_IFD ExposureBiasValue ;			/* SRATIONAL(1)	O	Exposure bias */
	T_BF_DCF_EXIF_IFD MaxApertureValue ;			/* RATIONAL(1)	O	Maximum lens aperture */
	T_BF_DCF_EXIF_IFD MeteringMode ;				/* SHORT(1)	O	Metering mode */
	T_BF_DCF_EXIF_IFD LightSource ;					/* SHORT(1)	O	Light source */
	T_BF_DCF_EXIF_IFD Flash ;						/* SHORT(1)	R	Flash */
/**/    	T_BF_DCF_EXIF_IFD FocalLength ;			/* RATIONAL(1)	O	Lens focal length */
	T_BF_DCF_EXIF_IFD MakerNote ;					/* UNDEFINED(any)	O	Manufacturer notes */
	T_BF_DCF_EXIF_IFD FlashPixVersion ;				/* UNDEFINED(4)	M	Supported FlashPix version */
	T_BF_DCF_EXIF_IFD ColorSpace ;					/* SHORT(1)	M	Color space information */
	T_BF_DCF_EXIF_IFD PixelXDimension ;				/* SHORT(1) or LONG(1)	M	Valid image width */
	T_BF_DCF_EXIF_IFD PixelYDimension ;				/* SHORT(1) or LONG(1)	M	Valid image height */
	T_BF_DCF_EXIF_IFD InteroperabilityIFDPointer ; 	/* LONG(1)	O	Pointer of Interoperability IFD */
	T_BF_DCF_EXIF_IFD SensingMethod ;				/* SHORT(1)	O	Sensing method */
	T_BF_DCF_EXIF_IFD FileSource ;					/* UNDEFINED(1)	O	File source */
	T_BF_DCF_EXIF_IFD SceneType ;					/* UNDEFINED(1)	O	Scene type */
	T_BF_DCF_EXIF_IFD ExposureMode ;				/* SHORT(1)	O	Exposure mode */
	T_BF_DCF_EXIF_IFD WhiteBalanceMode ;			/* SHORT(1)	O	White balance mode */
	T_BF_DCF_EXIF_IFD DigitalZoomRatio ;			/* RATIONAL(1)  	O	Digital zoom ratio */
	T_BF_DCF_EXIF_IFD FocalLength35mmFilm	;		/* SHORT(1)	O	FocalLength35mmFilm */
	T_BF_DCF_EXIF_IFD CaptureType ;					/* SHORT(1)	O	Capture Type */
/**/    	T_BF_DCF_EXIF_IFD Sharpness ;			/* SHORT(1)	O	Shrpness direction */
	FW_ULONG NextIFDOffset ;

	/* Value */
	T_BF_DCF_EXIF_RATIONAL  ValueExposureTime ;
	T_BF_DCF_EXIF_RATIONAL  ValueFNumber ;
	DCF_ASCII	  ValueDateTimeOriginal[D_BF_DCF_EXIF_V_DATEORIG_S] ;
	DCF_ASCII	  ValueDateTimeDigitized[D_BF_DCF_EXIF_V_DATEORIG_S] ;
	T_BF_DCF_EXIF_RATIONAL  ValueCompressedBitsPerPixel ;
	T_BF_DCF_EXIF_SRATIONAL ValueShutterSpeedValue ;
	T_BF_DCF_EXIF_RATIONAL  ValueApertureValue ;
	T_BF_DCF_EXIF_SRATIONAL ValueExposureBiasValue ;
	T_BF_DCF_EXIF_RATIONAL  ValueMaxApertureValue ;
   	T_BF_DCF_EXIF_RATIONAL  ValueFocalLength;
	T_BF_DCF_EXIF_RATIONAL  ValueDigitalZoomRatio ;
} T_BF_DCF_EXIF_JPEG_EXIF_IFD ;

/**
 * @breif R98 IFD of Exif
 * @note none
 * @attention none
 */
typedef struct {
	T_BF_DCF_EXIF_IFD InteroperabilityIndex ;	/* ASCII(1)	M */
	T_BF_DCF_EXIF_IFD InteroperabilityVersion;	/* UNDEFINED(4)	M */

	FW_ULONG NextIFDOffset ;
} T_BF_DCF_EXIF_JPEG_R98_IFD ;

/**
 * @breif 1st IFD of Exif
 * @note none
 * @attention none
 */
typedef struct {
	T_BF_DCF_EXIF_IFD Compression ;					/* SHORT(1)	M	Compression scheme */
	T_BF_DCF_EXIF_IFD Orientation ;					/* SHORT(1)	O	Orientation of image */
	T_BF_DCF_EXIF_IFD XResolution ;					/* RATIONAL(1)	M	Image resolution in width direction */
	T_BF_DCF_EXIF_IFD YResolution ;					/* RATIONAL(1)	M	Image resolution in height direction */
	T_BF_DCF_EXIF_IFD ResolutionUnit ;				/* SHORT(1)	M	Unit of X and Y resolution */
	T_BF_DCF_EXIF_IFD JPEGInterchangeFormat ;		/* LONG(1)	M	JPEGInterchangeFormat */
	T_BF_DCF_EXIF_IFD JPEGInterchangeFormatLength ;	/* LONG(1)	M	JPEGInterchangeFormatLength */

	FW_ULONG NextIFDOffset ;

	T_BF_DCF_EXIF_RATIONAL ValueXResolution ;
	T_BF_DCF_EXIF_RATIONAL ValueYResolution ;
} T_BF_DCF_EXIF_JPEG_FIRST_IFD ;

/**
 * @breif Jpeg header of Exif
 * @note none
 * @attention none
 */
typedef struct {
	DCF_SHORT Soi;
	T_BF_DCF_EXIF_APP1_MARKER App1;
	T_BF_DCF_EXIF_TIFF_HEADER Tiff_Head;
	DCF_SHORT Ifd0_Num;
	T_BF_DCF_EXIF_JPEG_ZERO_IFD Ifd0;
	DCF_SHORT ExifIfd_Num;
	T_BF_DCF_EXIF_JPEG_EXIF_IFD ExifIfd;
	DCF_SHORT R98Ifd_Num;
	T_BF_DCF_EXIF_JPEG_R98_IFD R98Ifd;
	DCF_SHORT Ifd1_Num;
	T_BF_DCF_EXIF_JPEG_FIRST_IFD Ifd1;
} T_BF_DCF_EXIF_JPEG_HEADER;

typedef struct {
	DCF_SHORT Soi;
	T_BF_DCF_EXIF_APP1_MARKER App1;
	T_BF_DCF_EXIF_TIFF_HEADER Tiff_Head;
	DCF_SHORT Ifd0_Num;
	T_BF_DCF_EXIF_JPEG_ZERO_IFD Ifd0;
	DCF_SHORT ExifIfd_Num;
	T_BF_DCF_EXIF_JPEG_EXIF_IFD ExifIfd;
} T_BF_DCF_EXIF_JPEG_HEADER_FIRST;

typedef struct {
	DCF_SHORT R98Ifd_Num;
	T_BF_DCF_EXIF_JPEG_R98_IFD R98Ifd;
	DCF_SHORT Ifd1_Num;
	T_BF_DCF_EXIF_JPEG_FIRST_IFD Ifd1;
} T_BF_DCF_EXIF_JPEG_HEADER_SECOND; 

#else

typedef struct {
	/*  Field Name                      Type         (Count)    Tag Name                  */
	/*                                                       M:Mandatory               */
	/*                                                       R:Conditionally mandatory */
	/*                                                       O:Optional                */
	T_BF_DCF_EXIF_IFD ImageDescription;				/* ASCII     (Any)	R	Image title */
	T_BF_DCF_EXIF_IFD Make;							/* ASCII     (Any)	R	Image input equipment manufacturer */
	T_BF_DCF_EXIF_IFD Model;						/* ASCII     (Any)	R	Image input equipment model */
	T_BF_DCF_EXIF_IFD Orientation;					/* SHORT     (1)	R	Orientation of image */
	T_BF_DCF_EXIF_IFD XResolution;					/* RATIONAL  (1)	M	Image resolution in width direction */
	T_BF_DCF_EXIF_IFD YResolution;					/* RATIONAL  (1)	M	Image resolution in height direction */
	T_BF_DCF_EXIF_IFD ResolutionUnit;				/* SHORT     (1)	M	Unit of X and Y resolution */
	T_BF_DCF_EXIF_IFD Software;						/* ASCII     (Any)	O	Software used */
	T_BF_DCF_EXIF_IFD DateTime;						/* ASCII     (20)	R	File change date and time */
	T_BF_DCF_EXIF_IFD YCbCrPositioning;				/* SHORT     (1)	M	Y and C positioning */
	T_BF_DCF_EXIF_IFD Copyright;					/* ASCII     (Any)	O	Copyright holder */
	T_BF_DCF_EXIF_IFD ExifIFDPointer;				/* LONG      (1)	M	Exif Tag */
#ifdef CO_BF_DCF_IF_SET_GPSTAG
	T_BF_DCF_EXIF_IFD GpsIFDPointer;				/* LONG      (1)	M	Gps Tag */
#endif
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	T_BF_DCF_EXIF_IFD PrintIM;						/* ASCII     (Any)	O	PrintIM version300 */
#endif

	FW_ULONG offset_to_1stIFD;			/* offset to 1st IFD */
} T_BF_DCF_EXIF_JPEG_ZERO_IFD;

typedef struct {
	DCF_ASCII      ValueImageDescription[D_BF_DCF_EXIF_V_DESCRIPTION_S];
	DCF_ASCII      ValueMake[D_BF_DCF_EXIF_V_MAKE_S];
	DCF_ASCII      ValueModel[D_BF_DCF_EXIF_V_MODEL_S];
	T_BF_DCF_EXIF_RATIONAL ValueXResolution;
	T_BF_DCF_EXIF_RATIONAL ValueYResolution;
	DCF_ASCII      ValueSoftware[D_BF_DCF_EXIF_V_SOFT_S];
	DCF_ASCII      ValueDateTime[D_BF_DCF_EXIF_V_DATETIME_S];
	DCF_ASCII      ValueCopyright[D_BF_DCF_EXIF_V_COPYRIGHT_S];
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	DCF_ASCII      ValuePrintIM[D_BF_DCF_EXIF_V_PRINTIM_S];
#endif
} T_BF_DCF_EXIF_JPEG_ZERO_VALUE;

/**
 * @breif Exif IFD of Exif
 * @note none
 * @attention none
 */
typedef struct {
	T_BF_DCF_EXIF_IFD ExposureTime;					/* RATIONAL  (1)	R	Exposure time */
	T_BF_DCF_EXIF_IFD FNumber;						/* RATIONAL  (1)	O	F number */
	T_BF_DCF_EXIF_IFD ExposureProgram;				/* SHORT     (1)	O	Exposure program */
	T_BF_DCF_EXIF_IFD ISOSpeedRatings;				/* SHORT     (1)	O	ISO speed ratings */
	T_BF_DCF_EXIF_IFD ExifVersion;					/* UNDEFINED (4)	M	Exif version */
	T_BF_DCF_EXIF_IFD DateTimeOriginal;				/* ASCII     (20)	O	Date and time of original data generation */
	T_BF_DCF_EXIF_IFD DateTimeDigitized;			/* ASCII     (20)	O	Date and time of digital data generation */
	T_BF_DCF_EXIF_IFD ComponentsConfiguration;		/* UNDEFINED (4)	M	Meaning of each component */
	T_BF_DCF_EXIF_IFD CompressedBitsPerPixel;		/* RATIONAL  (1)	O	Image compression mode */
	T_BF_DCF_EXIF_IFD ShutterSpeedValue;			/* SRATIONAL (1) 	O	Shutter speed */
	T_BF_DCF_EXIF_IFD ApertureValue;				/* RATIONAL  (1)	O	Aperture */
	T_BF_DCF_EXIF_IFD ExposureBiasValue;			/* SRATIONAL (1)	O	Exposure bias */
	T_BF_DCF_EXIF_IFD MaxApertureValue;				/* RATIONAL  (1)	O	Maximum lens aperture */
	T_BF_DCF_EXIF_IFD MeteringMode;					/* SHORT     (1)	O	Metering mode */
	T_BF_DCF_EXIF_IFD LightSource;					/* SHORT     (1)	O	Light source */
	T_BF_DCF_EXIF_IFD Flash;						/* SHORT     (1)	R	Flash */
	T_BF_DCF_EXIF_IFD FocalLength;					/* RATIONAL  (1)	O	Lens focal length */
	T_BF_DCF_EXIF_IFD MakerNote;					/* UNDEFINED (any)	O	Manufacturer notes */
	T_BF_DCF_EXIF_IFD FlashPixVersion;				/* UNDEFINED (4)	M	Supported FlashPix version */
	T_BF_DCF_EXIF_IFD ColorSpace;					/* SHORT     (1)	M	Color space information */
	T_BF_DCF_EXIF_IFD PixelXDimension;				/* SHORT/LONG(1)	M	Valid image width */
	T_BF_DCF_EXIF_IFD PixelYDimension;				/* SHORT/LONG(1)	M	Valid image height */
	T_BF_DCF_EXIF_IFD InteroperabilityIFDPointer;	/* LONG      (1)	O	Pointer of Interoperability IFD */
	T_BF_DCF_EXIF_IFD SensingMethod;				/* SHORT     (1)	O	Sensing method */
	T_BF_DCF_EXIF_IFD FileSource;					/* UNDEFINED (1)	O	File source */
	T_BF_DCF_EXIF_IFD SceneType;					/* UNDEFINED (1)	O	Scene type */
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	T_BF_DCF_EXIF_IFD CustomRendered;				/* SHORT     (1)	O	Custom image processing */
#endif
	T_BF_DCF_EXIF_IFD ExposureMode;					/* SHORT     (1)	O	Exposure mode */
	T_BF_DCF_EXIF_IFD WhiteBalanceMode;				/* SHORT     (1)	O	White balance mode */
	T_BF_DCF_EXIF_IFD DigitalZoomRatio;				/* RATIONAL  (1)  	O	Digital zoom ratio */
	T_BF_DCF_EXIF_IFD FocalLength35mmFilm;			/* SHORT     (1)	O	FocalLength35mmFilm */
	T_BF_DCF_EXIF_IFD CaptureType;					/* SHORT     (1)	O	Capture Type */
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	T_BF_DCF_EXIF_IFD Contrast;						/* SHORT     (1)	O	Contrast */
	T_BF_DCF_EXIF_IFD Saturation;					/* SHORT     (1)	O	Saturation */
#endif
	T_BF_DCF_EXIF_IFD Sharpness;					/* SHORT     (1)	O	Shrpness direction */
	FW_ULONG NextIFDOffset;
} T_BF_DCF_EXIF_JPEG_EXIF_IFD;

typedef struct {
	/* Value */
	T_BF_DCF_EXIF_RATIONAL  ValueExposureTime;
	T_BF_DCF_EXIF_RATIONAL  ValueFNumber;
	DCF_ASCII       ValueDateTimeOriginal[D_BF_DCF_EXIF_V_DATEORIG_S];
	DCF_ASCII       ValueDateTimeDigitized[D_BF_DCF_EXIF_V_DATEORIG_S];
	T_BF_DCF_EXIF_RATIONAL  ValueCompressedBitsPerPixel;
	T_BF_DCF_EXIF_SRATIONAL ValueShutterSpeedValue;
	T_BF_DCF_EXIF_RATIONAL  ValueApertureValue;
	T_BF_DCF_EXIF_SRATIONAL ValueExposureBiasValue;
	T_BF_DCF_EXIF_RATIONAL  ValueMaxApertureValue;
   	T_BF_DCF_EXIF_RATIONAL  ValueFocalLength;
	T_BF_DCF_EXIF_RATIONAL  ValueDigitalZoomRatio;
} T_BF_DCF_EXIF_JPEG_EXIF_VALUE;

/**
 * @breif R98 IFD of Exif
 * @note none
 * @attention none
 */
typedef struct {
	T_BF_DCF_EXIF_IFD InteroperabilityIndex;		/* ASCII     (1)	M	Interoperability Index */
	T_BF_DCF_EXIF_IFD InteroperabilityVersion;		/* UNDEFINED (4)	M	Interoperability Version */
	FW_ULONG NextIFDOffset;
} T_BF_DCF_EXIF_JPEG_R98_IFD;

#ifdef CO_BF_DCF_IF_SET_GPSTAG
/**
 * @breif Gps IFD of Exif
 * @note none
 * @attention none
 */
typedef struct {
	T_BF_DCF_EXIF_IFD GPSLatitudeRef;
	T_BF_DCF_EXIF_IFD GPSLatitude;
	T_BF_DCF_EXIF_IFD GPSLongitudeRef;
	T_BF_DCF_EXIF_IFD GPSLongitude;
	T_BF_DCF_EXIF_IFD GPSImgDirectionRef;
	T_BF_DCF_EXIF_IFD GPSImgDirection;
	FW_ULONG NextIFDOffset;
} T_BF_DCF_EXIF_JPEG_GPS_IFD;

typedef struct {
	T_BF_DCF_EXIF_RATIONAL ValueGPSLatitude[3];
	T_BF_DCF_EXIF_RATIONAL ValueGPSLongitude[3];
	T_BF_DCF_EXIF_RATIONAL ValueGPSImgDirection;
} T_BF_DCF_EXIF_JPEG_GPS_VALUE;
#endif

/**
 * @breif 1st IFD of Exif
 * @note none
 * @attention none
 */
typedef struct {
	T_BF_DCF_EXIF_IFD Compression;					/* SHORT     (1)	M	Compression scheme */
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	T_BF_DCF_EXIF_IFD Maker1;						/* ASCII     (Any)	R	Image input equipment manufacturer */
	T_BF_DCF_EXIF_IFD Model1;						/* ASCII     (Any)	R	Image input equipment model */
#endif
	T_BF_DCF_EXIF_IFD Orientation;					/* SHORT     (1)	O	Orientation of image */
	T_BF_DCF_EXIF_IFD XResolution;					/* RATIONAL  (1)	M	Image resolution in width direction */
	T_BF_DCF_EXIF_IFD YResolution;					/* RATIONAL  (1)	M	Image resolution in height direction */
	T_BF_DCF_EXIF_IFD ResolutionUnit;				/* SHORT     (1)	M	Unit of X and Y resolution */
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	T_BF_DCF_EXIF_IFD DateTime1;					/* ASCII     (20)	R	File change date and time */
#endif
	T_BF_DCF_EXIF_IFD JPEGInterchangeFormat;		/* LONG      (1)	M	JPEGInterchangeFormat */
	T_BF_DCF_EXIF_IFD JPEGInterchangeFormatLength;	/* LONG      (1)	M	JPEGInterchangeFormatLength */
	FW_ULONG NextIFDOffset;
} T_BF_DCF_EXIF_JPEG_FIRST_IFD;

typedef struct {
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	DCF_ASCII      ValueMaker1[D_BF_DCF_EXIF_V_MAKER1_S];
	DCF_ASCII      ValueModel1[D_BF_DCF_EXIF_V_MODEL1_S];
#endif
	T_BF_DCF_EXIF_RATIONAL ValueXResolution;
	T_BF_DCF_EXIF_RATIONAL ValueYResolution;
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	DCF_ASCII      ValueDateTime1[D_BF_DCF_EXIF_V_DATETIME1_S];
#endif
} T_BF_DCF_EXIF_JPEG_FIRST_VALUE;

typedef struct {
	DCF_SHORT						Soi;
	T_BF_DCF_EXIF_APP1_MARKER		App1;
	T_BF_DCF_EXIF_TIFF_HEADER		Tiff_Head;
	DCF_SHORT						Ifd0_Num;
	T_BF_DCF_EXIF_JPEG_ZERO_IFD		Ifd0;
	T_BF_DCF_EXIF_JPEG_ZERO_VALUE	Ifd0_value;
	DCF_SHORT						ExifIfd_Num;
	T_BF_DCF_EXIF_JPEG_EXIF_IFD		ExifIfd;
    T_BF_DCF_EXIF_JPEG_EXIF_VALUE	ExifIfd_value;
} T_BF_DCF_EXIF_JPEG_HEADER_FIRST;

typedef struct {
	DCF_SHORT						R98Ifd_Num;
	T_BF_DCF_EXIF_JPEG_R98_IFD		R98Ifd;
#ifdef CO_BF_DCF_IF_SET_GPSTAG
	DCF_SHORT						Gps_Num;
	T_BF_DCF_EXIF_JPEG_GPS_IFD		Gps;
	T_BF_DCF_EXIF_JPEG_GPS_VALUE	Gps_value;
#endif
	DCF_SHORT						Ifd1_Num;
	T_BF_DCF_EXIF_JPEG_FIRST_IFD	Ifd1;
	T_BF_DCF_EXIF_JPEG_FIRST_VALUE	Ifd1_value;
} T_BF_DCF_EXIF_JPEG_HEADER_SECOND;


#endif

/**
 * @breif Exif Orientaion Setting Informaion
 * @note none
 * @attention none
 */
typedef struct {
	FW_ULONG  zero_orient_offset;
	FW_ULONG  first_orient_offet;
	FW_ULONG  edit_date_offet;
	FW_ULONG  edit_date_offet_thumb;
	FW_USHORT orient_value;
	FW_USHORT first_orient_value;
} T_BF_DCF_EXIF_ORIENTATION_INFO;

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @breif Exif informaion of individual image
 * @note none
 * @attention none
 */
typedef struct {
	FW_ULONG  thumb_offset;
	FW_ULONG  thumb_length_offset;
	FW_ULONG  thumb_length_value;
	FW_ULONG  main_offset;
	FW_USHORT app2_size;
	FW_USHORT endian;
	FW_ULONG  class1_offset;
	FW_ULONG  class2_offset;
} T_BF_DCF_EXIF_EXT_MP_IMAGE_SIZE_INFO;
#endif      // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

/**
 * @breif Exif X_size & Y_size Setting Informaion
 * @note none
 * @attention none
 */
typedef struct {
	FW_ULONG  field_length_offset;
	FW_ULONG  thumb_offset;
	FW_ULONG  zero_orient_offset;
	FW_ULONG  first_orient_offset;
	FW_ULONG  exposure_time_offset;
	FW_ULONG  f_number_offset;
	FW_ULONG  iso_speed_rate_offset;
	FW_ULONG  date_time_original_offset;
	FW_ULONG  date_time_digit_offset;
	FW_ULONG  exposure_bias_value_offset;
	FW_ULONG  metering_mode_offset;
	FW_ULONG  flash_offset;
	FW_ULONG  white_balance_offset;
	FW_ULONG  edit_date_thumb_offset;
	FW_ULONG  x_size_offset;
	FW_ULONG  y_size_offset;
	FW_ULONG  edit_date_offset;
	FW_ULONG  thumb_length_offset;
	FW_USHORT field_length_value;
	FW_ULONG  x_size_value;
	FW_ULONG  y_size_value;
	FW_CHAR   edit_date_value[D_BF_DCF_EXIF_V_DATETIME_S];
	FW_ULONG  thumb_length_value;
	FW_ULONG  maker_note_offset;
#ifdef CO_BF_DCF_IF_MPF_ENABLE
	FW_USHORT app2_size;
	FW_USHORT endian;
	FW_ULONG  class1_offset;
	FW_ULONG  class2_offset;
#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
	T_BF_DCF_EXIF_EXT_MP_IMAGE_SIZE_INFO ext_mp[E_BF_DCF_IF_MPF_EXT_MP_SELECT_MAX];
	FW_ULONG ext_mp_num;
	FW_CHAR ext_mp_type;
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
#endif	// CO_BF_DCF_IF_MPF_ENABLE
} T_BF_DCF_EXIF_IMAGE_SIZE_INFO;

/* not depend DCF I/F(without file access) */
/**
 * @breif information by analizing Exif
 */
typedef struct {
	FW_ULONG     thumb_offset;
	FW_ULONG     thumb_size;
	FW_ULONG     main_offset; 			// for tiff
	E_BF_DCF_IF_ROTATE rotate;				// rotation direction
	E_BF_DCF_IF_ROTATE rotate_thumb;			// rotation direction
	FW_USHORT    width;					// image width
	FW_USHORT    lines;					// image length
	FW_USHORT    iso;					// iso value
	FW_USHORT    flash;					// flash on/off
	FW_ULONG     EpTimeNum;				// Exposure Time Numerator(TV)
	FW_ULONG     EpTimeDen;				// Exposure Time Denominator(TV)
	FW_ULONG     FvNum;					// F Value Numerator(AV)
	FW_ULONG     FvDen;					// F Value Denominator(AV)
	T_BF_DCF_IF_DATE   edit_date_time;		// edit date and time
} T_BF_DCF_EXIF_IMAGE_INFO;

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
//-----------------------------------------------------------------------------
// for MPF
//-----------------------------------------------------------------------------
typedef struct {
	DCF_SHORT app2_marker;
	DCF_SHORT field_length;
	DCF_ASCII mpf_id_code[4];
} T_BF_DCF_EXIF_APP2_MARKER_MPF;


typedef struct {
	T_BF_DCF_EXIF_IFD    mp_format_version;
	T_BF_DCF_EXIF_IFD    mp_entry_num;
	T_BF_DCF_EXIF_IFD    mp_entry;
	FW_ULONG next_ifd_pointer;
} T_BF_DCF_EXIF_MPF_INDEX_IFD;

typedef struct {
	T_BF_DCF_EXIF_APP2_MARKER_MPF	app2;
	T_BF_DCF_EXIF_MP_HEADER			mp_head;
	DCF_SHORT						mp_index_ifd_num;
	T_BF_DCF_EXIF_MPF_INDEX_IFD		mp_index_ifd;
} T_BF_DCF_EXIF_MPF_BASELINE_APP2;

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
typedef struct {
	T_BF_DCF_EXIF_IFD    mp_format_version;
	T_BF_DCF_EXIF_IFD    mp_individual_num;
	T_BF_DCF_EXIF_IFD    base_viewpoint_num;
	FW_ULONG next_ifd_pointer;
} T_BF_DCF_EXIF_MPF_MP_ATTRIBUTES_IFD;

typedef struct {
	T_BF_DCF_EXIF_APP2_MARKER_MPF	app2;
	T_BF_DCF_EXIF_MP_HEADER			mp_head;
	DCF_SHORT						mp_index_ifd_num;
	T_BF_DCF_EXIF_MPF_INDEX_IFD		mp_index_ifd;
	DCF_SHORT						mp_attribute_ifd_num;
	T_BF_DCF_EXIF_MPF_MP_ATTRIBUTES_IFD    mp_attribute_ifd;
} T_BF_DCF_EXIF_MPF_EXTENDED_FIRST_APP2;

typedef struct {
	T_BF_DCF_EXIF_APP2_MARKER_MPF	app2;
	T_BF_DCF_EXIF_MP_HEADER			mp_head;
	DCF_SHORT						        mp_attribute_ifd_num;
	T_BF_DCF_EXIF_MPF_MP_ATTRIBUTES_IFD mp_attribute_ifd;
} T_BF_DCF_EXIF_MPF_EXTENDED_APP2;
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

typedef struct {
	FW_ULONG  mp_info;
	FW_ULONG  mp_data_size;
	FW_ULONG  mp_data_offset;
	FW_USHORT mp_entry_1;
	FW_USHORT mp_entry_2;
} T_BF_DCF_EXIF_MP_ENTRY;

typedef struct {
	T_BF_DCF_EXIF_IFD    XResolution;
	T_BF_DCF_EXIF_IFD    YResolution;
	T_BF_DCF_EXIF_IFD    ResolutionUnit;
	T_BF_DCF_EXIF_IFD    ExifIFDPointer;
	FW_ULONG offset_to_1stIFD;
} T_BF_DCF_EXIF_MPF_SCR_ZERO_IFD;

typedef struct {
	T_BF_DCF_EXIF_RATIONAL ValueXResolution;
	T_BF_DCF_EXIF_RATIONAL ValueYResolution;
} T_BF_DCF_EXIF_MPF_SCR_ZERO_IFD_VALUE;

typedef struct {
	T_BF_DCF_EXIF_IFD    MakerNote;
	T_BF_DCF_EXIF_IFD    PixelXDimension;
	T_BF_DCF_EXIF_IFD    PixelYDimension;
	T_BF_DCF_EXIF_IFD    ImageUID;
	FW_ULONG NextIFDOffset;
} T_BF_DCF_EXIF_MPF_SCR_EXIF_IFD;

typedef struct {
	T_BF_DCF_EXIF_IFD    JPEGInterchangeFormat;
	T_BF_DCF_EXIF_IFD    JPEGInterchangeFormatLength;
	FW_ULONG NextIFDOffset;
} T_BF_DCF_EXIF_MPF_SCR_FIRST_IFD;

typedef struct {
	FW_USHORT								soi;
	T_BF_DCF_EXIF_APP1_MARKER				app1;
	T_BF_DCF_EXIF_TIFF_HEADER				tiff_head;
	FW_USHORT								ifd0_num;
	T_BF_DCF_EXIF_MPF_SCR_ZERO_IFD			ifd0;
	T_BF_DCF_EXIF_MPF_SCR_ZERO_IFD_VALUE	ifd0_value;
	FW_USHORT								exif_ifd_num;
	T_BF_DCF_EXIF_MPF_SCR_EXIF_IFD			exif_ifd;
	FW_UCHAR								image_uid_value[33];
	FW_USHORT								ifd1_num;
	T_BF_DCF_EXIF_MPF_SCR_FIRST_IFD			ifd1;
} T_BF_DCF_EXIF_MPF_APP1;

#endif // CO_BF_DCF_IF_MPF_ENABLE

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
//FW_INT32 BF_Dcf_Exif_Make_Jpeg(FW_ULONG thumb_size, FW_ULONG main_size, FW_ULONG vga_size, T_BF_DCF_EXIF_JPEG_HEADER** jpg_head);T_BF_DCF_IF_JPEG_DATA* jpg_data
FW_INT32 BF_Dcf_Exif_Make_Jpeg(T_BF_DCF_IF_JPEG_DATA* jpg_data);
FW_INT32 BF_Dcf_Exif_Make_Thm(T_BF_DCF_IF_THM_DATA* thm_data);
FW_INT32 BF_Dcf_Exif_Search_Half_Word_Code(FW_UCHAR* add, FW_ULONG size, FW_USHORT code, FW_ULONG start_offset, FW_ULONG* value_offset);
FW_VOID  BF_Dcf_Exif_Exchange_Datetime(T_BF_DCF_IF_DATE datetype, FW_UCHAR* addr);
FW_INT32 BF_Dcf_Exif_Analyze_Jpeg(FW_INT32 file_id, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data, T_BF_DCF_IF_IMAGE_DETAIL* img_detail);
FW_INT32 BF_Dcf_Exif_Analyze_Jpeg_Date(FW_INT32 file_id, T_BF_DCF_IF_DATE* datetime_buf);
FW_INT32 BF_Dcf_Exif_Get_Orientation_Offset_Jpeg(FW_INT32 file_id, T_BF_DCF_EXIF_ORIENTATION_INFO* exif_orient_info);
FW_INT32 BF_Dcf_Exif_Get_Image_Size_Offset_Jpeg(FW_INT32 file_id, T_BF_DCF_EXIF_IMAGE_SIZE_INFO* exif_image_size_info);
FW_INT32 BF_Dcf_Exif_Get_Image_Offset_Jpeg(FW_INT32 file_id, FW_ULONG* main_offset, FW_ULONG* thumb_offset, FW_ULONG* vga_offset);

FW_VOID  BF_Dcf_Exif_Copy_Exif_Jpeg(FW_CHAR* tmp_buff, FW_ULONG* jpg_head_length);
FW_VOID  BF_Dcf_Exif_Copy_Exif_Thm(FW_CHAR* tmp_buff, FW_ULONG* jpg_head_length);
FW_INT32 BF_Dcf_Exif_Set_Value( FW_USHORT ifd, FW_USHORT tag, FW_USHORT type, FW_USHORT count, FW_UCHAR* addr );

FW_INT32 BF_Dcf_Exif_Analyze_Jpeg_Resize(FW_INT32 file_id, FW_CHAR copy_flag, T_BF_DCF_EXIF_IMAGE_SIZE_INFO* exif_image_size_info);
FW_INT32 BF_Dcf_Exif_Reset_Jpeg_Resize(FW_VOID);
FW_INT32 BF_Dcf_Exif_Copy_Jpeg_Resize(FW_INT32 src_file_id, FW_INT32 dst_file_id, T_BF_DCF_EXIF_IMAGE_SIZE_INFO* exif_image_size_info);

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
//-----------------------------------------------------------------------------
// for MPF
//-----------------------------------------------------------------------------
#if 1
FW_VOID  BF_Dcf_Exif_Copy_App2_Mpf(FW_CHAR* tmp_buff, FW_ULONG* mpf_app2_length, FW_ULONG* mp_entry_offset, FW_USHORT mp_entry_num, FW_USHORT endian);
#else
FW_VOID  BF_Dcf_Exif_Copy_App2_Mpf(FW_CHAR* tmp_buff, FW_ULONG* mpf_app2_length, FW_ULONG* mp_entry_offset, FW_USHORT mp_entry_num);
#endif

#if 1
FW_VOID  BF_Dcf_Exif_Get_App1_Size_Mpf(FW_UCHAR mpf_index, FW_USHORT* mpf_app1_size);
FW_VOID  BF_Dcf_Exif_Make_Exif_Mpg(FW_UCHAR mpf_index, T_BF_DCF_IF_SCREEN_DATA* scr_data);
#if 1
FW_VOID  BF_Dcf_Exif_Copy_Exif_Mpf(FW_UCHAR mpf_index, FW_USHORT endian, FW_CHAR* tmp_buff, FW_ULONG* mpf_head_length);
#else
FW_VOID  BF_Dcf_Exif_Copy_Exif_Mpf(FW_UCHAR mpf_index, FW_CHAR* tmp_buff, FW_ULONG* mpf_head_length);
#endif
FW_VOID  BF_Dcf_Exif_Reset_App1_Mpf(FW_UCHAR mpf_index);
FW_INT32 BF_Dcf_Exif_Set_Value_Mpf(FW_UCHAR mpf_index, FW_USHORT ifd, FW_USHORT tag, FW_USHORT type, FW_USHORT count, FW_UCHAR* addr);
#else
FW_VOID  BF_Dcf_Exif_Make_Exif_Mpg(T_BF_DCF_IF_SCREEN_DATA* scr_data);
FW_VOID  BF_Dcf_Exif_Copy_Exif_Mpf(FW_CHAR class, FW_CHAR* tmp_buff, FW_ULONG* mpf_head_length);
FW_VOID  BF_Dcf_Exif_Reset_App1_Mpf(FW_VOID);
FW_INT32 BF_Dcf_Exif_Set_Value_Mpf(FW_USHORT ifd, FW_USHORT tag, FW_USHORT type, FW_USHORT count, FW_UCHAR* addr);
#endif
FW_VOID BF_Dcf_Exif_Reset_App2_Mpf(FW_VOID);
FW_VOID BF_Dcf_Exif_Exchange_Endian_Mpf(FW_UCHAR mpf_index, FW_USHORT endian);
FW_INT32 BF_Dcf_Exif_Analyze_Mpf_Resize(FW_UCHAR mpf_index, FW_INT32 file_id, FW_ULONG mpf_offset);
FW_VOID BF_Dcf_Exif_Exchange_Endian_Mpf_Entry(FW_USHORT endian, T_BF_DCF_EXIF_MP_ENTRY* mp_entry);
FW_VOID BF_Dcf_Exif_Exchange_Endian_Mpf_App2(FW_USHORT endian);
FW_VOID BF_Dcf_Exif_Reset_App1_Mpf_Resize(FW_UCHAR mpf_index);

FW_INT32 BF_Dcf_Exif_Get_App1_Size_Mpf_For_Buf(FW_UCHAR* top_addr, FW_ULONG* size);

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
FW_INT32 BF_Dcf_Exif_Make_Ext(T_BF_DCF_IF_EXT_MP_DATA* jpg_data);
FW_INT32 BF_Dcf_Exif_Analyze_Ext(FW_INT32 file_id, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data, T_BF_DCF_IF_IMAGE_DETAIL* img_detail);
FW_VOID BF_Dcf_Exif_Copy_Extended_First_App2_Mpf(FW_CHAR* tmp_buff, FW_ULONG* mpf_app2_length, FW_ULONG* mp_entry_offset, FW_USHORT mp_entry_num, FW_USHORT endian);
FW_VOID BF_Dcf_Exif_Copy_Extended_App2_Mpf(FW_CHAR* tmp_buff, FW_ULONG* mpf_app2_length, FW_USHORT endian);
FW_VOID BF_Dcf_Exif_Reset_Extended_First_App2_Mpf(FW_VOID);
FW_VOID BF_Dcf_Exif_Reset_Extended_App2_Mpf(FW_VOID);
FW_VOID BF_Dcf_Exif_Exchange_Endian_Mpf_Extended_First_App2(FW_USHORT endian);
FW_VOID BF_Dcf_Exif_Exchange_Endian_Mpf_Extended_App2(FW_USHORT endian);
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
#endif	// CO_BF_DCF_IF_MPF_ENABLE

FW_USHORT BF_Dcf_Exif_Get_Exif_Endian(FW_VOID);
FW_USHORT BF_Dcf_Exif_Exchange_App_Size(FW_USHORT app_size);

#endif // __EXIF_HEAD
