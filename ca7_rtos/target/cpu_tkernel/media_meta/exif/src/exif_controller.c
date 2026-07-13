/**
 * @file		dcf_exif.c
 * @brief		create DCF Exif tag format.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "exif_controller.h"
#include "mdf_wrapper.h"
#include "fj_host.h"
#include "debug.h"
#include "l1l2cache.h"
#include "category_parameter.h"


/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define D_BF_DCF_EXIF_TYPE_VALUE		(1)
#define D_BF_DCF_EXIF_TYPE_OFFSET		(2)

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN // CPU is BigEndian
#ifndef FJ_CUSTOM_EXIF_DATA_BIG_ENDIAN	// change exif endian of CPU endian
#define CO_BF_DCF_EXIF_CHANGE_ENDIAN  // EXIF create LittleEndian
#endif
#else // CPU is LittleEndian
#ifdef FJ_CUSTOM_EXIF_DATA_BIG_ENDIAN	// change exif endian of CPU endian
#define CO_BF_DCF_EXIF_CHANGE_ENDIAN // EXIF create BigEndian
#endif
#endif

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
#ifdef CO_BF_DCF_IF_GET_GPSTAG
/**
 * @breif GPS Latitude or Longitude information
 * @note none
 * @attention none
 */
typedef struct {
	FW_ULONG DdNum;		// GPS dd Numerator
	FW_ULONG DdDen;		// GPS dd Denominator
	FW_ULONG MmNum;		// GPS mm Numerator
	FW_ULONG MmDen;		// GPS mm Denominator
	FW_ULONG SsNum;		// GPS ss Numerator
	FW_ULONG SsDen;		// GPS ss Denominator
} T_BF_DCF_EXIF_GPS_LATITUDE_LONGITUDE;
#endif	// CO_BF_DCF_IF_GET_GPSTAG

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
static volatile FW_INT32 g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;	// big endian
#else
static volatile FW_INT32 g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;	// little endian
#endif




static volatile FW_INT32 gTagCheckCnt = 0;
static volatile FW_INT32 gNumOfTag = 0;

static volatile T_BF_DCF_EXIF_JPEG_HEADER_FIRST g_bf_dcf_exif_jpeg_header_first = {
	D_BF_DCF_EXIF_SOI, /* Marker prefix */
	/* APP1 marker */
	{
		D_BF_DCF_EXIF_APP1_MARKER, /* App1Marker */
		0x0000,	/* FieldLength */
		"Exif", /* ExifIdCode */
		0x00 /* Padding */
	},
	/* TIFF header of APP1 */
	{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
		D_BF_DCF_EXIF_LIT_EN, /* ByteOrder */
#else
		D_BF_DCF_EXIF_BIG_EN, /* ByteOrder */
#endif
#else
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
		D_BF_DCF_EXIF_BIG_EN, /* ByteOrder */
#else
		D_BF_DCF_EXIF_LIT_EN, /* ByteOrder */
#endif
#endif
		D_BF_DCF_EXIF_TIFF_HEARD_RESERVED, /* Reserved */
		D_BF_DCF_EXIF_TIFF_HEARD_OFFSET /* Offset */
	},
	D_BF_DCF_EXIF_IFD_NUM_OF_IFD0,
	/* 0th IFD of APP1 */
	{
		/* Tag Name             Tag ID                 Type Count                 Count                          Value/Offset */
		{/* ImageDescription */ FJ_IFD0_TAG_IMGDESC_,  D_BF_DCF_EXIF_ASCII_T,     D_BF_DCF_EXIF_V_DESCRIPTION_S, {D_BF_DCF_EXIF_O_DESCRIPTION}},
		{/* Make             */ FJ_IFD0_TAG_MAKE_,     D_BF_DCF_EXIF_ASCII_T,     D_BF_DCF_EXIF_V_MAKE_S,        {D_BF_DCF_EXIF_O_MAKE}},
		{/* Model            */ FJ_IFD0_TAG_MODEL_,    D_BF_DCF_EXIF_ASCII_T,     D_BF_DCF_EXIF_V_MODEL_S,       {D_BF_DCF_EXIF_O_MODEL}},
		{/* Orientation      */ FJ_IFD0_TAG_ORIENT_,   D_BF_DCF_EXIF_SHORT_T,     1,                             {D_BF_DCF_EXIF_DVAL_ORIENT}}, /* SValue = {1,0} */
		{/* XResolution      */ FJ_IFD0_TAG_XRES_,     D_BF_DCF_EXIF_RATIONAL_T,  1,                             {D_BF_DCF_EXIF_O_XRESOL}},
		{/* YResolution      */ FJ_IFD0_TAG_YRES_,     D_BF_DCF_EXIF_RATIONAL_T,  1,                             {D_BF_DCF_EXIF_O_YRESOL}},
		{/* ResolutionUnit   */ FJ_IFD0_TAG_RESUNIT_,  D_BF_DCF_EXIF_SHORT_T,     1,                             {D_BF_DCF_EXIF_DVAL_RESOLUNIT}}, /* SValue = {2,0} */
		{/* Software         */ FJ_IFD0_TAG_SOFT_,     D_BF_DCF_EXIF_ASCII_T,     D_BF_DCF_EXIF_V_SOFT_S,        {D_BF_DCF_EXIF_O_SOFT}},
		{/* DateTime         */ FJ_IFD0_TAG_DATETIME_, D_BF_DCF_EXIF_ASCII_T,     D_BF_DCF_EXIF_V_DATETIME_S,    {D_BF_DCF_EXIF_O_DATETIME}},
		{/* YCbCrPositioning */ FJ_IFD0_TAG_YCBCRPOS_, D_BF_DCF_EXIF_SHORT_T,     1,                             {D_BF_DCF_EXIF_DVAL_YCBCRPOS}}, /* SValue = {2,0} */
		{/* Copyright        */FJ_IFD0_TAG_COPYRIGHT_, D_BF_DCF_EXIF_ASCII_T,     D_BF_DCF_EXIF_V_COPYRIGHT_S,   {D_BF_DCF_EXIF_O_COPYRIGHT}},
		{/* ExifIFDPointer   */FJ_IFD0_TAG_EXIFPTR_,   D_BF_DCF_EXIF_LONG_T,      1,                             {D_BF_DCF_EXIF_O_EXIFPOINTER}},
#ifdef CO_BF_DCF_IF_SET_GPSTAG
		{/* GpsIFDPointer	 */ FJ_IFD0_TAG_GPSPTR_,   D_BF_DCF_EXIF_LONG_T,      1,							 {D_BF_DCF_EXIF_GPS_IFD_POINTER}},
#endif
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		{/* PrintIM Ver-0300 */FJ_IFD0_TAG_PRINTIM_,   D_BF_DCF_EXIF_UNDEFINED_T, D_BF_DCF_EXIF_V_PRINTIM_S,     {D_BF_DCF_EXIF_O_PRINTIM}},
#endif
		D_BF_DCF_EXIF_NEXT_IFD_POINTER, /* offset_to_1stIFD */
	},
	{
		
		{/* value of ImageDescription */FJ_CUSTOM_EXIF_IMG_DESCRIPTION,		/* 28 byte =27+1(NULL) */},//pgr0506
		{/* value of Make             */FJ_CUSTOM_EXIF_MAKE,					/* 28 byte */},
		{/* value of Model            */FJ_CUSTOM_EXIF_MODEL,					/*  8 byte */},//pgr0506
		/* value of XResolution      */{72,1},						/*  8 byte */
		/* value of YResolution      */{72,1},						/*  8 byte */
		{/* value of Software         */FJ_CUSTOM_EXIF_SOFT,					/*  8 byte */},
		{/* value of DateTime         */"    :  :     :  :  ",		/* 20 byte */},
		{/* value of Copyright        */FJ_CUSTOM_EXIF_COPYRIGHT				/* 16 byte */},
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		{/* value of PrintIM          */FJ_CUSTOM_EXIF_PRINTIM,					/* 28 byte */},//pgr0506
#endif
	},

	D_BF_DCF_EXIF_IFD_NUM_OF_IFDEXIF,
    /* Exif IFD of APP1 */
	{
		/* Tag Name                      Tag ID                        Type                     Count, Value/Offset */
		{/* ExposureTime               */FJ_EXIF_TAG_EXPTIME_,         D_BF_DCF_EXIF_RATIONAL_T,  1, {D_BF_DCF_EXIF_O_EXPTIME} },
		{/* FNumber                    */FJ_EXIF_TAG_FNUM_,            D_BF_DCF_EXIF_RATIONAL_T,  1, {D_BF_DCF_EXIF_O_FNUMBER} },
		{/* ExposureProgram            */FJ_EXIF_TAG_EXPPROG_,         D_BF_DCF_EXIF_SHORT_T,     1, {D_BF_DCF_EXIF_DVAL_EXPOSPROG}},/* 2:normal program */
		{/* ISOSpeedRatings            */FJ_EXIF_TAG_ISOSPEEDRATE_,    D_BF_DCF_EXIF_SHORT_T,     1, {D_BF_DCF_EXIF_DVAL_ISOSPEED}},/* Default:100 */
		{/* ExifVersion                */FJ_EXIF_TAG_EXIFVER_,         D_BF_DCF_EXIF_UNDEFINED_T, 4, {D_BF_DCF_EXIF_DVAL_EXIFVER}},/* UValue[4]="0221" */
		{/* DateTimeOriginal           */FJ_EXIF_TAG_DATETIMEORIG_,    D_BF_DCF_EXIF_ASCII_T,     D_BF_DCF_EXIF_V_DATEORIG_S, {D_BF_DCF_EXIF_O_DATEORIG} },
		{/* DateTimeDigitized          */FJ_EXIF_TAG_DATETIMEDITI_,    D_BF_DCF_EXIF_ASCII_T,     D_BF_DCF_EXIF_V_DATEDIGI_S, {D_BF_DCF_EXIF_O_DATEDIGI} },
		{/* ComponentsConfiguration    */FJ_EXIF_TAG_COMPCONFIGURE_,   D_BF_DCF_EXIF_UNDEFINED_T, 4, {D_BF_DCF_EXIF_DVAL_COMPONENTCONFIG}},/* 1230(YCbCr) */
		{/* CompressedBitsPerPixel     */FJ_EXIF_TAG_COMPRESSBPP_,     D_BF_DCF_EXIF_RATIONAL_T,  1, {D_BF_DCF_EXIF_O_BITSPERPIXEL} },
		{/* ShutterSpeedValue          */FJ_EXIF_TAG_SHUTTER_,         D_BF_DCF_EXIF_SRATIONAL_T, 1, {D_BF_DCF_EXIF_O_SHUTTER} },
		{/* ApertureValue              */FJ_EXIF_TAG_APERTURE_,        D_BF_DCF_EXIF_RATIONAL_T,  1, {D_BF_DCF_EXIF_O_APERTUE} },
		{/* ExposureBiasValue          */FJ_EXIF_TAG_EXPBIAS_,         D_BF_DCF_EXIF_SRATIONAL_T, 1, {D_BF_DCF_EXIF_O_EXPOSUREBIAS} },
		{/* MaxApertureValue           */FJ_EXIF_TAG_MAXAPTURE_,       D_BF_DCF_EXIF_RATIONAL_T,  1, {D_BF_DCF_EXIF_O_MAXAPERTUER} },
		{/* MeteringMode               */FJ_EXIF_TAG_METERMODE_,       D_BF_DCF_EXIF_SHORT_T,     1, {0x00000000} },
		{/* LightSource                */FJ_EXIF_TAG_LIGHTSOURCE_,     D_BF_DCF_EXIF_SHORT_T,     1, {0x00000000} },/* 0:AUTO(unknown) */
		{/* Flash                      */FJ_EXIF_TAG_FLASH_,           D_BF_DCF_EXIF_SHORT_T,     1, {0x00000000}  },/* 0:did not fire */
		{/* FocalLength				   */FJ_EXIF_TAG_FOCALLEN_,        D_BF_DCF_EXIF_RATIONAL_T,  1, {D_BF_DCF_EXIF_O_FOCAL} },
		{/* MakerNote                  */FJ_EXIF_TAG_MAKERNOTE_,       D_BF_DCF_EXIF_UNDEFINED_T, 0, {D_BF_DCF_EXIF_O_MAKERNOTE} },
		{/* FlashPixVersion            */FJ_EXIF_TAG_FLRESHPIXVER_,    D_BF_DCF_EXIF_UNDEFINED_T, 4, {D_BF_DCF_EXIF_DVAL_FLASHPIXVER}},/* UValue[4]="0100" */
		{/* ColorSpace                 */FJ_EXIF_TAG_COLORSPACE_,      D_BF_DCF_EXIF_SHORT_T,     1, {D_BF_DCF_EXIF_DVAL_COLORSPACE}},/* 1:sRGB */
		{/* PixelXDimension            */FJ_EXIF_TAG_PEXELXDIM_,       D_BF_DCF_EXIF_LONG_T,      1, {0} },
		{/* PixelYDimension            */FJ_EXIF_TAG_PEXELYDIM_,       D_BF_DCF_EXIF_LONG_T,      1, {0} },
		{/* InteroperabilityIFDPointer */FJ_EXIF_TAG_ITOPIFDPTR_,      D_BF_DCF_EXIF_LONG_T,      1, {D_BF_DCF_EXIF_O_R98POINTER} },
		{/* SensingMethod              */FJ_EXIF_TAG_SENSOR_,          D_BF_DCF_EXIF_SHORT_T,     1, {D_BF_DCF_EXIF_DVAL_SENSMETHOD} },/* 2:One-chip color area sensor */
		{/* FileSource                 */FJ_EXIF_TAG_FILESOURCE_,      D_BF_DCF_EXIF_UNDEFINED_T, 1, {D_BF_DCF_EXIF_DVAL_FILESOURCE} },/* 3:DSC */
		{/* SceneType                  */FJ_EXIF_TAG_SENCETYPE_,       D_BF_DCF_EXIF_UNDEFINED_T, 1, {D_BF_DCF_EXIF_DVAL_SCENETYPE} },/* 1:A directly photographed image */
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		{/* CustomRendered             */FJ_EXIF_TAG_CUSTOMRENDERED_,  D_BF_DCF_EXIF_SHORT_T,     1, {0x00000000} },/* 0:normal process */
#endif
		{/* ExposureMode               */FJ_EXIF_TAG_EXPOSUREMODE_,    D_BF_DCF_EXIF_SHORT_T,     1, {0x00000000} },/* 0:auto exposure */
		{/* WhiteBalance               */FJ_EXIF_TAG_WB_,              D_BF_DCF_EXIF_SHORT_T,     1, {0x00000000} },/* 0:auto white balance */
		{/* DigitalZoomRatio           */FJ_EXIF_TAG_DZOOM_,           D_BF_DCF_EXIF_RATIONAL_T,  1, {D_BF_DCF_EXIF_O_DIGIZOOM} },
		{/* FocalLength35mmFilm        */FJ_EXIF_TAG_FOCALLENGTH35mm_, D_BF_DCF_EXIF_SHORT_T,     1, {0x00000000} },
		{/* CaptureType                */FJ_EXIF_TAG_CAPTURETYPE_,     D_BF_DCF_EXIF_SHORT_T,     1, {0x00000000} },
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		{/* Contrast                   */FJ_EXIF_TAG_CONTRAST_,        D_BF_DCF_EXIF_SHORT_T,     1, {0x00000000} },/* normal */
		{/* Saturation                 */FJ_EXIF_TAG_SATURATION_,      D_BF_DCF_EXIF_SHORT_T,     1, {0x00000000} },/* normal */
#endif
		{/* Sharpness				   */FJ_EXIF_TAG_SHARPNESS_,       D_BF_DCF_EXIF_SHORT_T,     1, {0x00000000} },/* 0:Normal */

		/* Next IFD Offset */   0x00000000,
	},
	{
		/* value of ExposureTime           */{1,60},
		/* value of FNumber                */{29,10},
		/* value of DateTimeOriginal       */{"    :  :     :  :  "},
		/* value of DateTimeDigitized      */{"    :  :     :  :  "},//pgr0506
		/* value of CompressedBitsPerPixel */{5,1},/* 固定 */
		/* value of ShutterSpeedValue      */{1,1},
		/* value of ApertureValue          */{29,10},
		/* value of ExposureBiasValue      */{0,10},
		/* value of MaxApertureValue       */{28,10},
		/* value of Lens focal length	   */{1,10},
		/* value of DigitalZoomRatio       */{1,1}
	}
};

static volatile T_BF_DCF_EXIF_JPEG_HEADER_SECOND g_bf_dcf_exif_jpeg_header_second = {
	D_BF_DCF_EXIF_IFD_NUM_OF_R98,
	{ /* ExifR98 IFD(ExifR98 Information) */
		/* InteroperabilityIndex   */{FJ_ITOP_TAG_ITOPINDEX_,   D_BF_DCF_EXIF_ASCII_T,     4, {D_BF_DCF_EXIF_DVAL_INTEROPEINDEX}},/* UValue="R98" */
		/* InteroperabilityVersion */{FJ_ITOP_TAG_ITOPVERSION_, D_BF_DCF_EXIF_UNDEFINED_T, 4, {D_BF_DCF_EXIF_DVAL_INTEROPEVER}},/* UValue="0100" */
		0x00000000 /* Next IFD Offset */
	},
#ifdef CO_BF_DCF_IF_SET_GPSTAG
	D_BF_DCF_EXIF_IFD_NUM_OF_GPS,
    /* Gps IFD of APP1 */
	{
		/* Tag Name                       Tag ID                           Type                      Count, Value/Offset */
		{/* GPSLatitudeRef         */     FJ_GPS_TAG_GPSLATITUDEREF_,      D_BF_DCF_EXIF_ASCII_T,    2,     {0x00000000}},
		{/* GPSLatitude            */     FJ_GPS_TAG_GPSLATITUDE_,         D_BF_DCF_EXIF_RATIONAL_T, 3,     {D_BF_DCF_EXIF_O_GPSLATITUDE}},
		{/* GPSLongitudeRef        */     FJ_GPS_TAG_GPSLONGITUDEREF_,     D_BF_DCF_EXIF_ASCII_T,    2,     {0x00000000}},
		{/* GPSLongitude           */     FJ_GPS_TAG_GPSLONGITUDE_,        D_BF_DCF_EXIF_RATIONAL_T, 3,     {D_BF_DCF_EXIF_O_GPSLONGITUDE}},
		{/* GPSImgDirectionRef     */     FJ_GPS_TAG_GPSIMGDIRECTIONREF_,  D_BF_DCF_EXIF_ASCII_T,    2,     {0x00000000}},
		{/* GPSImgDirection        */     FJ_GPS_TAG_GPSIMGDIRECTION_,     D_BF_DCF_EXIF_RATIONAL_T, 1,     {D_BF_DCF_EXIF_O_GPSIMGDIRECTION}},
		/* Next IFD Offset */   0x00000000
	},
	{
		/* value of GPSLatitude */{{0,0},{0,0},{0,0}},
		/* value of GPSLongitude */{{0,0},{0,0},{0,0}},
		/* value of GPSImgDirection */{0,0}
	},
#endif
	D_BF_DCF_EXIF_IFD_NUM_OF_IFD1,
    /* 1st IFD of APP1 (information of thumbnail data) */
	{
		/* Tag Name                       Tag ID                           Type                    Count, Value/Offset */
		{/* Compression                 */FJ_IFD1_TAG_COMPRESS_,           D_BF_DCF_EXIF_SHORT_T,    1, {D_BF_DCF_EXIF_DVAL_COMPRESSION}},/* SValue={6,0} */
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		{/* Maker1                      */FJ_IFD1_TAG_MAKER1_,             D_BF_DCF_EXIF_ASCII_T,    D_BF_DCF_EXIF_V_MAKER1_S, {D_BF_DCF_EXIF_O_MAKER1}},
		{/* Model1                      */FJ_IFD1_TAG_MODEL1_,             D_BF_DCF_EXIF_ASCII_T,    D_BF_DCF_EXIF_V_MODEL1_S, {D_BF_DCF_EXIF_O_MODEL1}},
#endif
		{/* Orientation                 */FJ_IFD1_TAG_ORIENT_,             D_BF_DCF_EXIF_SHORT_T,    1, {D_BF_DCF_EXIF_DVAL_ORIENT}},
		{/* XResolution                 */FJ_IFD1_TAG_XRES_,               D_BF_DCF_EXIF_RATIONAL_T, 1, {D_BF_DCF_EXIF_O_XRES_1ST}},
		{/* YResolution                 */FJ_IFD1_TAG_YRES_,               D_BF_DCF_EXIF_RATIONAL_T, 1, {D_BF_DCF_EXIF_O_YRES_1ST}},
		{/* ResolutionUnit              */FJ_IFD1_TAG_RESUINT_,            D_BF_DCF_EXIF_SHORT_T,    1, {D_BF_DCF_EXIF_DVAL_RESOLUNIT}},/* SValue={2,0} */
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		{/* DateTime1                   */FJ_IFD1_TAG_DATETIME1_,          D_BF_DCF_EXIF_ASCII_T,    D_BF_DCF_EXIF_V_DATETIME1_S, {D_BF_DCF_EXIF_O_DATETIME1}},
#endif
		{/* JPEGInterchangeFormat       */FJ_IFD1_TAG_JPG_INTERCHGFMT_,    D_BF_DCF_EXIF_LONG_T,     1, {D_BF_DCF_EXIF_THUMBNAIL_DATA_OFFSET}}, /* thumbnail data starts at H'600 */
		{/* JPEGInterchangeFormatLength */FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_, D_BF_DCF_EXIF_LONG_T,     1, {0}},

		/* Next IFD Offset */   0x00000000,
	},
	{
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		{/* value of Maker1      */FJ_CUSTOM_EXIF_MAKER1,				/* 28 byte */},
		{/* value of Model1      */FJ_CUSTOM_EXIF_MODEL1,				/*  8 byte */},
#endif
		/* value of XResolution */{72,1},
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		/* value of YResolution */{72,1},
		/* value of DateTime1   */{"    :  :     :  :  "}
#else
		/* value of YResolution */{72,1}
#endif
	}
};

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
//----------------------------------------------------------------------------
// for MPF
//----------------------------------------------------------------------------
static volatile T_BF_DCF_EXIF_MPF_BASELINE_APP2 g_bf_dcf_exif_mpf_app2 =  {
	// APP2 marker
	{
		D_BF_DCF_EXIF_APP2_MARKER,		// APP2 marker
		0,						// field length
		"MPF"					// MP ID code
	},
	// MP Header
	{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
		D_BF_DCF_EXIF_BIG_EN,					// byte order
#else
		D_BF_DCF_EXIF_LIT_EN,					// byte order
#endif
		D_BF_DCF_EXIF_MP_HEAD_RESERVED,	// reserved
		D_BF_DCF_EXIF_MP_HEAD_OFFSET		// offset to 1st IFD
	},
	D_BF_DCF_EXIF_MP_INDEX_IFD_NUM,	// MP index IFD num
	// MP Index IFD
	{
	//   Tag ID         Type   Count            Value/Offset
		{0xB000, D_BF_DCF_EXIF_UNDEFINED_T,      4,             {D_BF_DCF_EXIF_DVAL_MPFVER}},	// MP format Version ("0100")
		{0xB001,      D_BF_DCF_EXIF_LONG_T,      1,             {1}},	// MP Entry Num
		{0xB002, D_BF_DCF_EXIF_UNDEFINED_T,     16,             {D_BF_DCF_EXIF_MP_ENTRY_OFFSET}},	// MP Entry
		0x00000000				// Next IFD offset
	}
};

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
static volatile T_BF_DCF_EXIF_MPF_EXTENDED_FIRST_APP2 g_bf_dcf_exif_mpf_ext_first_app2 =  {
	// APP2 marker
	{
		D_BF_DCF_EXIF_APP2_MARKER,		// APP2 marker
		0,						// field length
		"MPF"					// MP ID code
	},
	// MP Header
	{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
		D_BF_DCF_EXIF_BIG_EN,					// byte order
#else
		D_BF_DCF_EXIF_LIT_EN,					// byte order
#endif
		D_BF_DCF_EXIF_MP_HEAD_RESERVED,	// reserved
		D_BF_DCF_EXIF_MP_HEAD_OFFSET		// offset to 1st IFD
	},
	D_BF_DCF_EXIF_MP_INDEX_IFD_NUM,	// MP index IFD num
	// MP Index IFD
	{
	//   Tag ID         Type   Count            Value/Offset
		{0xB000, D_BF_DCF_EXIF_UNDEFINED_T,      4,             {D_BF_DCF_EXIF_DVAL_MPFVER}},	// MP format Version ("0100")
		{0xB001,      D_BF_DCF_EXIF_LONG_T,      1,             {1}},	// MP Entry Num
		{0xB002, D_BF_DCF_EXIF_UNDEFINED_T,     16,             {D_BF_DCF_EXIF_MP_ENTRY_OFFSET}},	// MP Entry
		0x00000000				// Next IFD offset
	},
	D_BF_DCF_EXIF_EXT_MP_ATTRIBUTE_IFD_NUM,	// MP attribute IFD num
	// MP Attribute IFD
	{
	//   Tag ID         Type   Count            Value/Offset
		{0xB000,      D_BF_DCF_EXIF_UNDEFINED_T,      4,             {D_BF_DCF_EXIF_DVAL_MPFVER}},	// MP format Version ("0100")
		{0xB101,      D_BF_DCF_EXIF_LONG_T,           1,             {1}},	// MP Individual Image Number
		{0xB204,      D_BF_DCF_EXIF_LONG_T,           1,             {0xFFFFFFFF}},	// Base Viewpoint Number
		0x00000000				// Next IFD offset
	}
};

static volatile T_BF_DCF_EXIF_MPF_EXTENDED_APP2 g_bf_dcf_exif_mpf_ext_app2 =  {
	// APP2 marker
	{
		D_BF_DCF_EXIF_APP2_MARKER,		// APP2 marker
		0,						// field length
		"MPF"					// MP ID code
	},
	// MP Header
	{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
		D_BF_DCF_EXIF_BIG_EN,					// byte order
#else
		D_BF_DCF_EXIF_LIT_EN,					// byte order
#endif
		D_BF_DCF_EXIF_MP_HEAD_RESERVED,	// reserved
		D_BF_DCF_EXIF_MP_HEAD_OFFSET		// offset to 1st IFD
	},
	D_BF_DCF_EXIF_EXT_MP_ATTRIBUTE_IFD_NUM,	// MP attribute IFD num
	// MP Attribute IFD
	{
	//   Tag ID         Type   Count            Value/Offset
		{0xB000,      D_BF_DCF_EXIF_UNDEFINED_T,      4,             {D_BF_DCF_EXIF_DVAL_MPFVER}},	// MP format Version ("0100")
		{0xB101,      D_BF_DCF_EXIF_LONG_T,           1,             {1}},	// MP Individual Image Number
		{0xB204,      D_BF_DCF_EXIF_LONG_T,           1,             {0xFFFFFFFF}},	// Base Viewpoint Number
		0x00000000				// Next IFD offset
	}
};
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

#if 1
static volatile T_BF_DCF_EXIF_MPF_APP1 g_bf_dcf_exif_mpf_screen_app1[2] = {
	{
	D_BF_DCF_EXIF_SOI,
	// APP1 marker
	{
		D_BF_DCF_EXIF_APP1_MARKER,											// App1Marker
		0x0000,																// FieldLength
		"Exif",																// ExifIdCode
		0x00																// Padding
	},
	// TIFF header of APP1
	{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
		D_BF_DCF_EXIF_LIT_EN,																// ByteOrder
#else
		D_BF_DCF_EXIF_BIG_EN,																// ByteOrder
#endif
#else
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
		D_BF_DCF_EXIF_BIG_EN,																// ByteOrder
#else
		D_BF_DCF_EXIF_LIT_EN,																// ByteOrder
#endif
#endif
		D_BF_DCF_EXIF_TIFF_HEARD_RESERVED,											// Reserved
		D_BF_DCF_EXIF_TIFF_HEARD_OFFSET												// Offset
	},
	D_BF_DCF_EXIF_MPF_IFD0_NUM,													// IFD0 num
	// IFD0
	{
		//         Tag ID                Type                    Count, Value/Offset
		{/*0x011A*/0x0000,               D_BF_DCF_EXIF_RATIONAL_T, 1,   {0}},	// XResolution
		{/*0x011B*/0x0000,               D_BF_DCF_EXIF_RATIONAL_T, 1,   {0}},	// YResolution
		{/*0x0128*/0x0000,               D_BF_DCF_EXIF_SHORT_T,    1,   {D_BF_DCF_EXIF_DVAL_RESOLUNIT}},	// ResolutionUnit
		{          FJ_IFD0_TAG_EXIFPTR_, D_BF_DCF_EXIF_LONG_T,     1,   {0}},	// ExifIFDPointer
		0x00000000															// Next IFD Pointer
	},
	// IFD0 value
	{
		{72, 1},															// value of XResolution
		{72, 1}																// value of YResolution
	},
	D_BF_DCF_EXIF_MPF_EXIFIFD_NUM,													// Exif IFD num
	// Exif IFD
	{
		//         Tag ID                  Type                                   Count       Value/Offset
		{/*0x927C*/0x0000,                 D_BF_DCF_EXIF_UNDEFINED_T,               0,             {0}},	// MakerNote
		{          FJ_EXIF_TAG_PEXELXDIM_, D_BF_DCF_EXIF_LONG_T,                    1,             {0}},	// PixelXDimension
		{          FJ_EXIF_TAG_PEXELYDIM_, D_BF_DCF_EXIF_LONG_T,                    1,             {0}},	// PixelYDimension
		{/*0xA420*/0x0000,                 D_BF_DCF_EXIF_UNDEFINED_T, D_BF_DCF_EXIF_IMAGEUID_SIZE, {0}},	// ImageUID
		0x00000000															// Next IFD Offset
	},
	// Image UID value
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	0,																		// IFD1 num
	{
		//         Tag ID         Type                                 Count    Value/Offset
		{/*0x0201*/0x0000,      D_BF_DCF_EXIF_LONG_T,                    1,          {0}},	// JPEGInterchangeFormat
		{/*0x0202*/0x0000,      D_BF_DCF_EXIF_LONG_T,                    1,          {0}},	// JPEGInterchangeFormatLength
		0x00000000															// Next IFD Offset
	}
	},
	{
	D_BF_DCF_EXIF_SOI,
	// APP1 marker
	{
		D_BF_DCF_EXIF_APP1_MARKER,													// App1Marker
		0x0000,																// FieldLength
		"Exif",																// ExifIdCode
		0x00																// Padding
	},
	// TIFF header of APP1
	{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
		D_BF_DCF_EXIF_LIT_EN,																// ByteOrder
#else
		D_BF_DCF_EXIF_BIG_EN,																// ByteOrder
#endif
#else
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
		D_BF_DCF_EXIF_BIG_EN,																// ByteOrder
#else
		D_BF_DCF_EXIF_LIT_EN,																// ByteOrder
#endif
#endif
		D_BF_DCF_EXIF_TIFF_HEARD_RESERVED,											// Reserved
		D_BF_DCF_EXIF_TIFF_HEARD_OFFSET												// Offset
	},
	D_BF_DCF_EXIF_MPF_IFD0_NUM,													// IFD0 num
	// IFD0
	{
		//         Tag ID                Type                    Count, Value/Offset
		{/*0x011A*/0x0000,               D_BF_DCF_EXIF_RATIONAL_T, 1, {0}},	// XResolution
		{/*0x011B*/0x0000,               D_BF_DCF_EXIF_RATIONAL_T, 1, {0}},	// YResolution
		{/*0x0128*/0x0000,               D_BF_DCF_EXIF_SHORT_T,    1, {D_BF_DCF_EXIF_DVAL_RESOLUNIT}},	// ResolutionUnit
		{          FJ_IFD0_TAG_EXIFPTR_, D_BF_DCF_EXIF_LONG_T,     1, {0}},	// ExifIFDPointer
		0x00000000															// Next IFD Pointer
	},
	// IFD0 value
	{
		{72, 1},															// value of XResolution
		{72, 1}																// value of YResolution
	},
	D_BF_DCF_EXIF_MPF_EXIFIFD_NUM,													// Exif IFD num
	// Exif IFD
	{
		//         Tag ID                  Type                                   Count       Value/Offset
		{/*0x927C*/0x0000,                 D_BF_DCF_EXIF_UNDEFINED_T,               0,             {0}},	// MakerNote
		{          FJ_EXIF_TAG_PEXELXDIM_, D_BF_DCF_EXIF_LONG_T,                    1,             {0}},	// PixelXDimension
		{          FJ_EXIF_TAG_PEXELYDIM_, D_BF_DCF_EXIF_LONG_T,                    1,             {0}},	// PixelYDimension
		{/*0xA420*/0x0000,                 D_BF_DCF_EXIF_UNDEFINED_T, D_BF_DCF_EXIF_IMAGEUID_SIZE, {0}},	// ImageUID
		0x00000000															// Next IFD Offset
	},
	// Image UID value
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	0,																		// IFD1 num
	{
		//         Tag ID         Type                                 Count     Value/Offset
		{/*0x0201*/0x0000,      D_BF_DCF_EXIF_LONG_T,                    1,          {0}},	// JPEGInterchangeFormat
		{/*0x0202*/0x0000,      D_BF_DCF_EXIF_LONG_T,                    1,          {0}},	// JPEGInterchangeFormatLength
		0x00000000															// Next IFD Offset
	}
	}
};
#else
static volatile T_BF_DCF_EXIF_MPF_APP1 g_bf_dcf_exif_mpf_screen_app1 = {
	D_BF_DCF_EXIF_SOI,
	// APP1 marker
	{
		D_BF_DCF_EXIF_APP1_MARKER,											// App1Marker
		0x0000,																// FieldLength
		"Exif",																// ExifIdCode
		0x00																// Padding
	},
	// TIFF header of APP1
	{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
		D_BF_DCF_EXIF_BIG_EN,														// ByteOrder
#else
		D_BF_DCF_EXIF_LIT_EN,														// ByteOrder
#endif
		D_BF_DCF_EXIF_TIFF_HEARD_RESERVED,											// Reserved
		D_BF_DCF_EXIF_TIFF_HEARD_OFFSET												// Offset
	},
	D_BF_DCF_EXIF_MPF_IFD0_NUM,													// IFD0 num
	// IFD0
	{
		//         Tag ID                Type                    Count    Value/Offset
		{/*0x011A*/0x0000,               D_BF_DCF_EXIF_RATIONAL_T, 1,          {0}},	// XResolution
		{/*0x011B*/0x0000,               D_BF_DCF_EXIF_RATIONAL_T, 1,          {0}},	// YResolution
		{/*0x0128*/0x0000,               D_BF_DCF_EXIF_SHORT_T,    1, {D_BF_DCF_EXIF_DVAL_RESOLUNIT}},	// ResolutionUnit
		{          FJ_IFD0_TAG_EXIFPTR_, D_BF_DCF_EXIF_LONG_T,     1,          {0}},	// ExifIFDPointer
		0x00000000															// Next IFD Pointer
	},
	// IFD0 value
	{
		{72, 1},															// value of XResolution
		{72, 1}																// value of YResolution
	},
	D_BF_DCF_EXIF_MPF_EXIFIFD_NUM,													// Exif IFD num
	// Exif IFD
	{
		//         Tag ID                  Type                                   Count       Value/Offset
		{/*0x927C*/0x0000,                 D_BF_DCF_EXIF_UNDEFINED_T,               0,             {0}},	// MakerNote
		{          FJ_EXIF_TAG_PEXELXDIM_, D_BF_DCF_EXIF_LONG_T,                    1,             {0}},	// PixelXDimension
		{          FJ_EXIF_TAG_PEXELYDIM_, D_BF_DCF_EXIF_LONG_T,                    1,             {0}},	// PixelYDimension
		{/*0xA420*/0x0000,                 D_BF_DCF_EXIF_LONG_T,      D_BF_DCF_EXIF_IMAGEUID_SIZE, {0}},	// ImageUID
		0x00000000															// Next IFD Offset
	},
	// Image UID value
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	0,																		// IFD1 num
	{
		//         Tag ID         Type                 Count Value/Offset
		{/*0x0201*/0x0000,      D_BF_DCF_EXIF_LONG_T,                    1,          {0}},	// JPEGInterchangeFormat
		{/*0x0202*/0x0000,      D_BF_DCF_EXIF_LONG_T,                    1,          {0}},	// JPEGInterchangeFormatLength
		0x00000000															// Next IFD Offset
	}
};
#endif

#endif	// CO_BF_DCF_IF_MPF_ENABLE

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/
#if 0
static FW_VOID make_1st_ifd_jpeg(FW_ULONG thumb_size);
static FW_VOID make_exif_ifd_jpeg(FW_ULONG thumb_size, FW_ULONG main_size, FW_ULONG vga_size);
static FW_VOID make_0th_ifd_jpeg(T_BF_DCF_IF_EXIF_INFO exif_info);
#endif
#ifdef CO_BF_FS_USER_CUSTOM_USE_BCD_DATE
static FW_VOID dcf_exif_set_bcd_datetime(FW_UCHAR* addr, T_BF_DCF_IF_DATE datetype);
#else
static FW_VOID dcf_exif_set_bin_datetime(FW_UCHAR* addr, T_BF_DCF_IF_DATE datetype);
#endif
static FW_USHORT dcf_exif_get_short_value(FW_UCHAR* add);
static FW_ULONG dcf_exif_get_long_value(FW_UCHAR* add);
static FW_USHORT dcf_exif_get_short_value_big_endian(FW_UCHAR* add);
static FW_INT32 dcf_exif_check_exif_tag(FW_INT32 file_id, FW_UCHAR* add, FW_USHORT tag_id, FW_ULONG* offset, FW_ULONG* reset_offset, FW_ULONG* value_offset, FW_ULONG* tatal_buf, FW_UCHAR* type, FW_ULONG* count);
static FW_INT32 dcf_exif_get_exif_set_value(FW_INT32 file_id, FW_UCHAR* add, FW_ULONG* value_offset, FW_ULONG tatal_buf, FW_ULONG* value1, FW_ULONG* value2);
#if 0
static FW_INT32 dcf_exif_get_exif_set_value_ex(FW_INT32 file_id, FW_UCHAR* add, FW_ULONG* value_offset, FW_ULONG tatal_buf, FW_LONG* value1, FW_LONG* value2);
#endif
static FW_INT32 dcf_exif_get_exif_date_value(FW_INT32 file_id, FW_UCHAR* add, FW_ULONG* value_offset, FW_ULONG tatal_buf, T_BF_DCF_IF_DATE* date);
static FW_USHORT dcf_exif_get_year(FW_CHAR* date);
static FW_UCHAR dcf_exif_get_month(FW_CHAR* date);
static FW_UCHAR dcf_exif_get_day(FW_CHAR* date);
static FW_UCHAR dcf_exif_get_hour(FW_CHAR* date);
static FW_UCHAR dcf_exif_get_min(FW_CHAR* date);
static FW_UCHAR dcf_exif_get_sec(FW_CHAR* date);
static FW_INT32 dcf_exif_get_exif_idf_offset(FW_INT32 file_id, FW_UCHAR* add, FW_ULONG idf_ptr, FW_ULONG* offset, FW_ULONG* reset_offset, FW_ULONG* tatal_buf);

static FW_VOID dcf_exif_delete_ifd0_tag( FW_USHORT size, FW_USHORT ifd, FW_USHORT tag );
static FW_VOID dcf_exif_delete_exif_tag( FW_USHORT size, FW_USHORT ifd, FW_USHORT tag );
static FW_VOID dcf_exif_delete_r98_tag( FW_USHORT size, FW_USHORT ifd, FW_USHORT tag  );
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
static FW_VOID dcf_exif_delete_ifd1_tag( FW_USHORT size, FW_USHORT ifd, FW_USHORT tag );
#endif

static FW_VOID dcf_exif_offset_reduce_ifd0( FW_USHORT size );
static FW_VOID dcf_exif_offset_reduce_exif( FW_USHORT size );
static FW_VOID dcf_exif_offset_reduce_ifd1( FW_USHORT size );

static FW_UCHAR dcf_exif_offset_reduce_value( FW_USHORT size, FW_USHORT ifd, FW_USHORT tag_id );

static FW_VOID dcf_exif_init_tag_offset( FW_VOID );

//static FW_VOID reset_resize_exif_value(FW_VOID);
static FW_VOID dcf_exif_exchange_endian_resize(FW_VOID);
static FW_VOID dcf_exif_tag_type_convert_byte( FW_UCHAR tagtype_val, FW_UCHAR* tagtype_byte );
static FW_VOID dcf_exif_get_sof_offset(FW_UCHAR* buffer, FW_ULONG* sof_offset);
static FW_INT32 dcf_exif_check_tag_value_type(FW_UCHAR type, FW_ULONG count, FW_CHAR* val_type);

#ifdef CO_BF_DCF_IF_GET_GPSTAG
static FW_INT32 dcf_exif_get_gps_coordinate_value(FW_INT32 file_id, FW_UCHAR* add, FW_ULONG* value_offset, FW_ULONG total_buf, T_BF_DCF_EXIF_GPS_LATITUDE_LONGITUDE* value);
#endif	// CO_BF_DCF_IF_GET_GPSTAG

static FW_INT32 dcf_exif_set_exif_tag_to_buf(T_BF_DCF_EXIF_IFD* tag_data, FW_CHAR* tmp_buf);
static FW_INT32 dcf_exif_set_exif_data_to_buf(FW_CHAR* tag_val, FW_UCHAR type, FW_UCHAR count, FW_CHAR* tmp_buf);
static FW_INT32 dcf_exif_set_exif_tiff_header_to_buf(T_BF_DCF_EXIF_TIFF_HEADER* tiff_val, FW_CHAR* tmp_buf);

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
//----------------------------------------------------------------------------
// for MPF
//----------------------------------------------------------------------------
#if 1
static FW_VOID dcf_exif_set_offset_mpf(FW_UCHAR mpf_index);
#else
static FW_VOID dcf_exif_set_offset_mpf(FW_VOID);
#endif
static FW_VOID dcf_exif_swap_long(FW_ULONG* data);
static FW_VOID dcf_exif_swap_short(FW_USHORT* data);
static FW_VOID dcf_exif_exchange_endian_mpf(FW_UCHAR mpf_index);
static FW_INT32 dcf_exif_get_ycbcr_format(FW_INT32 file_id, FW_UCHAR *buffer, FW_ULONG read_offset, FW_UCHAR *ycbcr_format);
#endif // CO_BF_DCF_IF_MPF_ENABLE

/**
 * @brief Get 2 byte value of target address.
 * @param i:*add target address
 * @return 2 byte value
 * @note none
 * @attention none.
 */
static FW_USHORT dcf_exif_get_short_value(FW_UCHAR* add)
{
	if (add == NULL) {
		return 0;
	}

	if (g_bf_dcf_exif_big_or_lit == D_BF_DCF_EXIF_BIG_EN) {
		return (((*add) << 8) + (*(add + 1)));
	}
	else {
		return ((*add) + ((*(add + 1)) << 8));
	}
}

/**
 * @brief Get 4 byte value of target address.
 * @param i:*add target address
 * @return 4 byte value
 * @note none
 * @attention none.
 */
static FW_ULONG dcf_exif_get_long_value(FW_UCHAR* add)
{
	if (add == NULL) {
		return 0;
	}

	if (g_bf_dcf_exif_big_or_lit == D_BF_DCF_EXIF_BIG_EN) {
		return (((*add) << 24) + ((*(add + 1)) << 16) + ((*(add + 2)) << 8) + (*(add + 3)));
	}
	else {
		return ((*add) + ((*(add + 1)) << 8) + ((*(add + 2)) << 16) + ((*(add + 3)) << 24));
	}
}

/**
 * @brief Get 2 byte value of target address.
 * @param i:*add target address
 * @return 2 byte value
 * @note Always get value by big-endian
 * @attention none.
 */
static FW_USHORT dcf_exif_get_short_value_big_endian(FW_UCHAR* add)
{
	if(add == NULL){
		return 0;
	}

	return (((*add)<<8) + (*(add+1)));
}


/**
 * @brief search exif tag offset in target buffer.
 * @param i:file_id file id
 * @param i:*add target address
 * @param i:tag_id tag id
 * @param i/o:*offset pointer of next tag offset
 * @param i/o:*reset_offset pointer of reset offset
 * @param i/o:*value_offset pointer of tag value offset
 * @param i/o:*tatal_buf pointer of total buffer size
 * @param o:*type pointer of tag type
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 dcf_exif_check_exif_tag(FW_INT32 file_id, FW_UCHAR* add, FW_USHORT tag_id, FW_ULONG* offset, FW_ULONG* reset_offset, FW_ULONG* value_offset, FW_ULONG* tatal_buf, FW_UCHAR* type, FW_ULONG* count)
{
	FW_INT32 ercd, check_cnt_bak;
	FW_ULONG result;

	if (add == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (offset == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (reset_offset == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (value_offset == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (tatal_buf == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (type == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (gNumOfTag < 1) {
		return FSDCF_ERRNO_EXIF_TAG_OVER;
	}
	
	check_cnt_bak = gTagCheckCnt;

	while (dcf_exif_get_short_value(add + *offset) != tag_id) {
		if (dcf_exif_get_short_value(add + *offset) > tag_id) {
			gTagCheckCnt = check_cnt_bak;
			return FSDCF_ERRNO_EXIF_TAG_OVER;
		}
		gTagCheckCnt++;
		if (gTagCheckCnt > gNumOfTag) {
			gTagCheckCnt = check_cnt_bak;
			return FSDCF_ERRNO_EXIF_TAG_OVER;
		}

		*offset += 12;
		if (*offset > D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE) {
			ercd = BF_Mdf_Wrapper_Read(file_id, add, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
			if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				return ercd;
			}

			*tatal_buf += result;
			*offset -= D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
			*reset_offset = *offset;
		}
	}

	if ((*offset + 2) > D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE) {
		*reset_offset = *offset + 12 - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
		*value_offset = *offset + 8 - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
		*offset = 0;
		ercd = BF_Mdf_Wrapper_Read(file_id, add, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			return ercd;
		}

		*tatal_buf += result;
		*type = dcf_exif_get_short_value( add + *offset + 2);
		*count = dcf_exif_get_long_value( add + *offset + 4);
	}
	else {
		*type = dcf_exif_get_short_value( add + *offset + 2);

		if ((*offset + 4) > D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE) {
			*reset_offset = *offset + 12 - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
			*value_offset = *offset + 8 - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
			*offset = 0;
			ercd = BF_Mdf_Wrapper_Read(file_id, add, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
			if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				return ercd;
			}

			*tatal_buf += result;
			*type = dcf_exif_get_short_value( add + *offset + 2);
			*count = dcf_exif_get_long_value( add + *offset + 4);
		}
		else {
			*count = dcf_exif_get_long_value( add + *offset + 4);

			if ((*offset + 8) > D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE) {
				*reset_offset = *offset + 12 - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
				*value_offset = *offset + 8 - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
				*offset = 0;
				ercd = BF_Mdf_Wrapper_Read(file_id, add, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
				if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
					return ercd;
				}
				*tatal_buf += result;
			}
			else {
				*reset_offset = *offset;
				*value_offset = 8;
			}
		}
	}

	return 0;
}

/**
 * @brief get exif tag value(set value) in target buffer.
 * @param i:file_id file id
 * @param i:*add target address
 * @param i/o:*value_offset pointer of tag value offset
 * @param i:*tatal_buf pointer of total buffer size
 * @param o:*value1 pointer of tag value1
 * @param o:*value2 pointer of tag value2
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 dcf_exif_get_exif_set_value(FW_INT32 file_id, FW_UCHAR* add, FW_ULONG* value_offset, FW_ULONG tatal_buf, FW_ULONG* value1, FW_ULONG* value2)
{
	FW_INT32  ercd;
	FW_UCHAR* tmp_buff;
	FW_ULONG  result;
	FW_ULONG  surplus;
	FW_ULONG  before_total,after_total;
	FW_INT32  ptr = 0;

	if (add == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (value_offset == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (value1 == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (value2 == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	before_total = tatal_buf;
	after_total = tatal_buf;

	if (*value_offset > tatal_buf) {
		ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
			return ercd;
		}

		ercd = BF_Mdf_Wrapper_Lseek(file_id, 0, D_BF_MDF_WRAPPER_SEEK_CUR, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			return ercd;
		}

		for (;;) {
			ercd = BF_Mdf_Wrapper_Read(file_id, tmp_buff, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
			if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
				return ercd;
			}

			after_total += result;
			if (after_total > *value_offset) {
				*value_offset -= before_total;
				break;
			}
			else {
				before_total += result;
			}
		}
		*value1 = dcf_exif_get_long_value(tmp_buff + *value_offset);
		*value2 = dcf_exif_get_long_value(tmp_buff + *value_offset + 4);

		ercd = BF_Mdf_Wrapper_Lseek(file_id, ptr, D_BF_MDF_WRAPPER_SEEK_SET, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			return ercd;
		}

		ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
			return ercd;
		}
	}
	else {
		surplus = (tatal_buf % D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		if (surplus == 0) {
			*value_offset -= (tatal_buf - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		else {
			*value_offset -= (tatal_buf - surplus);
		}
		*value1 = dcf_exif_get_long_value(add + *value_offset);
		*value2 = dcf_exif_get_long_value(add + *value_offset + 4);
	}

	return 0;
}

#if 0
/**
 * @brief get exif tag value(set value (FW_LONG)) in target buffer.
 * @param i:file_id file id
 * @param i:*add target address
 * @param i/o:*value_offset pointer of tag value offset
 * @param i:*tatal_buf pointer of total buffer size
 * @param o:*value1 pointer of tag value1
 * @param o:*value2 pointer of tag value2
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 dcf_exif_get_exif_set_value_ex(FW_INT32 file_id, FW_UCHAR* add, FW_ULONG* value_offset, FW_ULONG tatal_buf, FW_LONG* value1, FW_LONG* value2)
{
	FW_INT32  ercd;
	FW_UCHAR* tmp_buff;
	FW_ULONG  result;
	FW_ULONG  surplus;
	FW_ULONG  before_total, after_total;
	FW_INT32  ptr = 0;

	if (add == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (value_offset == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (value1 == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (value2 == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	before_total = tatal_buf;
	after_total = tatal_buf;

	if (*value_offset > tatal_buf) {
		ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
			return ercd;
		}

		ercd = BF_Mdf_Wrapper_Lseek(file_id, 0, D_BF_MDF_WRAPPER_SEEK_CUR, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			return ercd;
		}

		for (;;) {
			ercd = BF_Mdf_Wrapper_Read(file_id, tmp_buff, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
			if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
				return ercd;
			}

			after_total += result;
			if (after_total > *value_offset) {
				*value_offset -= before_total;
				break;
			}
			else {
				before_total += result;
			}
		}
		*value1 = dcf_exif_get_long_value(tmp_buff + *value_offset);
		*value2 = dcf_exif_get_long_value(tmp_buff + *value_offset + 4);

		ercd = BF_Mdf_Wrapper_Lseek(file_id, ptr, D_BF_MDF_WRAPPER_SEEK_SET, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			return ercd;
		}

		ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
			return ercd;
		}
	}
	else {
		surplus = (tatal_buf % D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		if (surplus == 0) {
			*value_offset -= (tatal_buf - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		else {
			*value_offset -= (tatal_buf - surplus);
		}
		*value1 = dcf_exif_get_long_value(add + *value_offset);
		*value2 = dcf_exif_get_long_value(add + *value_offset + 4);
	}

	return 0;
}
#endif

/**
 * @brief Get year from Exif setting date.
 * @param i:*date Exif date & time
 * @return 0001..9999 : year of Exif date
 *         0 : no year
 * @note none
 * @attention none.
 */
static FW_USHORT dcf_exif_get_year(FW_CHAR* date)
{
	FW_INT32 char_cnt;
	FW_SHORT year;

	if (date == NULL) {
		return 0;
	}

	// check charactor
	for (char_cnt = 0; char_cnt < 4; char_cnt++) {
		if (!isdigit((FW_INT32)(date[char_cnt]))) {
			return 0;
		}
	}
	if (isdigit((FW_INT32)(date[4]))) {
		return 0;
	}

	year = atoi(date);

	if ((year == 0) || (year > 9999)) {
		return 0;
	}
	else {
		return year;
	}
}

/**
 * @brief Get month from Exif setting date.
 * @param i:*date Exif date & time
 * @return 1..12 : month of Exif date
 *         0 : no month
 * @note none
 * @attention none.
 */
static FW_UCHAR dcf_exif_get_month(FW_CHAR* date)
{
	FW_INT32 char_cnt;
	FW_UCHAR month;

	if (date == NULL) {
		return 0;
	}

	// check charactor
	for (char_cnt = 5; char_cnt < 7; char_cnt++) {
		if (!isdigit((FW_INT32)(date[char_cnt]))) {
			return 0;
		}
	}
	if (isdigit((FW_INT32)(date[7]))) {
		return 0;
	}

	month = atoi(&date[5]);

	if ((month == 0) || (month > 12)) {
		return 0;
	}
	else {
		return month;
	}
}

/**
 * @brief Get day from Exif setting date.
 * @param i:*date Exif date & time
 * @return 1..31 : day of Exif date
 *         0 : no day
 * @note none
 * @attention none.
 */
static FW_UCHAR dcf_exif_get_day(FW_CHAR* date)
{
	FW_INT32 char_cnt;
	FW_UCHAR day;

	if (date == NULL) {
		return 0;
	}

	// check charactor
	for (char_cnt = 8; char_cnt < 10; char_cnt++) {
		if (!isdigit((FW_INT32)(date[char_cnt]))) {
			return 0;
		}
	}
	if (isdigit((FW_INT32)(date[10]))) {
		return 0;
	}

	day = atoi(&date[8]);

	if ((day == 0) || (day > 31)) {
		return 0;
	}
	else {
		return day;
	}
}

/**
 * @brief Get hour from Exif setting date.
 * @param i:*date Exif date & time
 * @return 0..24 : hour of Exif date
 * @note none
 * @attention none.
 */
static FW_UCHAR dcf_exif_get_hour(FW_CHAR* date)
{
	FW_INT32 char_cnt;
	FW_UCHAR hour;

	if (date == NULL) {
		return 0;
	}

	// check charactor
	for (char_cnt = 11; char_cnt < 13; char_cnt++) {
		if (!isdigit((FW_INT32)(date[char_cnt]))) {
			return 0;
		}
	}
	if (isdigit((FW_INT32)(date[13]))) {
		return 0;
	}

	hour = atoi(&date[11]);

	if ((hour == 0) || (hour > 24)) {
		return 0;
	}
	else {
		return hour;
	}
}

/**
 * @brief Get minute from Exif setting date.
 * @param i:*date Exif date & time
 * @return 0..59 : minute of Exif date
 * @note none
 * @attention none.
 */
static FW_UCHAR dcf_exif_get_min(FW_CHAR* date)
{
	FW_INT32 char_cnt;
	FW_UCHAR min;

	if (date == NULL) {
		return 0;
	}

	// check charactor
	for (char_cnt = 14; char_cnt < 16; char_cnt++) {
		if (!isdigit((FW_INT32)(date[char_cnt]))) {
			return 0;
		}
	}
	if (isdigit((FW_INT32)(date[16]))) {
		return 0;
	}

	min = atoi(&date[14]);

	if ((min == 0) || (min > 59)) {
		return 0;
	}
	else {
		return min;
	}
}

/**
 * @brief Get second from Exif setting date.
 * @param i:*date Exif date & time
 * @return 0..59 : second of Exif date
 * @note none
 * @attention none.
 */
static FW_UCHAR dcf_exif_get_sec(FW_CHAR* date)
{
	FW_INT32 char_cnt;
	FW_UCHAR sec;

	if (date == NULL) {
		return 0;
	}

	// check charactor
	for (char_cnt = 17; char_cnt < 19; char_cnt++) {
		if (!isdigit((FW_INT32)(date[char_cnt]))) {
			return 0;
		}
	}
	if (isdigit((FW_INT32)(date[19]))) {
		return 0;
	}

	sec = atoi(&date[17]);

	if ((sec == 0) || (sec > 59)) {
		return 0;
	}
	else {
		return sec;
	}
}

/**
 * @brief search exif tag offset in target buffer.
 * @param i:file_id file id
 * @param i:*add target address
 * @param i/o:*value_offset pointer of tag value offset
 * @param i:*tatal_buf pointer of total buffer size
 * @param o:*date pointer of date value
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 dcf_exif_get_exif_date_value(FW_INT32 file_id, FW_UCHAR* add, FW_ULONG* value_offset, FW_ULONG tatal_buf, T_BF_DCF_IF_DATE* date)
{
	FW_INT32 ercd;
	FW_CHAR* tmp_buff;
	FW_ULONG result;
	FW_ULONG surplus;
	FW_CHAR  date_time[20];
	FW_ULONG before_total, after_total;
	FW_INT32 ptr = 0;

	if (add == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (value_offset == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (date == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	before_total = tatal_buf;
	after_total = tatal_buf;

	if (*value_offset > tatal_buf) {
		ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
			return ercd;
		}

		ercd = BF_Mdf_Wrapper_Lseek(file_id, 0, D_BF_MDF_WRAPPER_SEEK_CUR, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			return ercd;
		}

		for (;;) {
			ercd = BF_Mdf_Wrapper_Read(file_id, tmp_buff, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
			if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
				return ercd;
			}

			after_total += result;
			if (after_total > *value_offset) {
				*value_offset -= before_total;
				break;
			}
			else {
				before_total += result;
			}
		}
		memcpy(date_time, (tmp_buff + *value_offset), 20);
		date->ad_year = dcf_exif_get_year(date_time);
		date->month = dcf_exif_get_month(date_time);
		date->day = dcf_exif_get_day(date_time);
		date->hour = dcf_exif_get_hour(date_time);
		date->min = dcf_exif_get_min(date_time);
		date->sec = dcf_exif_get_sec(date_time);

		ercd = BF_Mdf_Wrapper_Lseek(file_id, ptr, D_BF_MDF_WRAPPER_SEEK_SET, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			return ercd;
		}

		ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
			return ercd;
		}
	}
	else {
		surplus = (tatal_buf % D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		if (surplus == 0) {
			*value_offset -= (tatal_buf - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		else {
			*value_offset -= (tatal_buf - surplus);
		}
		memcpy(date_time, (add + *value_offset), 20);
		date->ad_year = dcf_exif_get_year(date_time);
		date->month = dcf_exif_get_month(date_time);
		date->day = dcf_exif_get_day(date_time);
		date->hour = dcf_exif_get_hour(date_time);
		date->min = dcf_exif_get_min(date_time);
		date->sec = dcf_exif_get_sec(date_time);
	}

	return 0;
}

/**
 * @brief search exif tag offset in target buffer.
 * @param i:file_id file id
 * @param i:*add target address
 * @param i:idf_ptr next IDF offset
 * @param i:*offset pointer of next ifd offset
 * @param i:*reset_offset pointer of reset offset
 * @param i:*tatal_buf pointer of total buffer size
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 dcf_exif_get_exif_idf_offset(FW_INT32 file_id, FW_UCHAR* add, FW_ULONG idf_ptr, FW_ULONG* offset, FW_ULONG* reset_offset, FW_ULONG* tatal_buf)
{
	FW_INT32 ercd;
	FW_ULONG result;
	FW_ULONG surplus;
	FW_ULONG tmp_ptr;
	FW_ULONG before_total, after_total;

	if (offset == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (reset_offset == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (tatal_buf == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	before_total = *tatal_buf;
	after_total = *tatal_buf;

	gTagCheckCnt = 1;	// reset seearch count

	tmp_ptr = idf_ptr + 2;

	if (tmp_ptr > *tatal_buf) {
		for (;;) {
			ercd = BF_Mdf_Wrapper_Read(file_id, add, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
			if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				return ercd;
			}
			after_total += result;
			if (after_total > tmp_ptr) {
				*offset = tmp_ptr - before_total;
				*reset_offset = *offset;
				*tatal_buf = after_total;
				break;
			}
			else {
				before_total += result;
			}
		}
	}
	else {
		surplus = ((*tatal_buf) % D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		if (surplus == 0) {
			*offset = tmp_ptr - ((*tatal_buf) - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		else {
			*offset = tmp_ptr - ((*tatal_buf) - surplus);
		}
		*reset_offset = *offset;
	}

	// Get xxx IFD Num of Tag
	gNumOfTag = dcf_exif_get_short_value((add + *offset) - 2);

	return 0;
}

/**
 * @brief set dummy exif data
 * @param i:*img_detail pointer
 * @return none
 * @note none
 * @attention none.
 */
static void dcf_exif_exif_info_dummy(T_BF_DCF_IF_IMAGE_DETAIL* img_detail)
{
	// orientation
	img_detail->rotate = E_BF_DCF_IF_ROTATE_0;

	// date & time
	memset(&img_detail->edit_date_time, 0x20, sizeof(T_BF_DCF_IF_DATE));

	// exposure time
	memset(&img_detail->EpTimeNum, 0x00, sizeof(FW_ULONG));
	memset(&img_detail->EpTimeDen, 0x00, sizeof(FW_ULONG));

	// F value
	memset(&img_detail->FvNum, 0x00, sizeof(FW_ULONG));
	memset(&img_detail->FvDen, 0x00, sizeof(FW_ULONG));

	// ISO speed rating
	img_detail->iso = 0x0;

	// flash
	img_detail->flash = 0;

	// image width
//	load_image_data->thumb_offset = jpeg_start_offset;

	// image height
//	load_image_data->thumb_size = 0x0;

	// orientation
	img_detail->rotate_thumb = E_BF_DCF_IF_ROTATE_0;

	// image size
	img_detail->width = 0;
	img_detail->lines = 0;
}
/**
 * @brief Get thumnal's YCC format from thumnal information
 * @param i:buffer buffer in which  thumnal's data are
 * @param i:buffer_size buffer's size
 * @return 0: error. 0x21:422. 0x22:0x22
 * @note none
 * @attention none.
 */
static BYTE dcf_exif_get_thum_ycc_format(const BYTE* buffer, FW_ULONG buffer_size)
{
	BYTE		ycc_format = 0;
	FW_ULONG	buffer_ptr;

	// search SOF(0xFFC0) for thumbnail exif
	if (buffer[3] == 0xC0) {
		ycc_format = buffer[13];
	}
	else {
		buffer_ptr = ((buffer[4] << 8) + buffer[5]) + 4;

		while (1) {
			if ((buffer[buffer_ptr] == 0xFF) && (buffer[buffer_ptr + 1] == 0xC0)) {		// check SOF marker
				ycc_format = buffer[buffer_ptr + 11];
				break;
			}

			buffer_ptr += ((buffer[buffer_ptr + 2] << 8) + buffer[buffer_ptr + 3]) + 2;
			if (buffer_ptr > buffer_size - 4) {
				break;	// not find SOF
			}
		}
	}

	return ycc_format;
}

/**
 * @brief search exif tag offset in target buffer.
 * @param i:*buffer data after the APPn area
 * @param i:*sof_offset sof ffset from the beginning of the buffer
 * @return error code
 * @note none
 * @attention none.
 */
static FW_VOID dcf_exif_get_sof_offset(FW_UCHAR* buffer, FW_ULONG* sof_offset)
{
	FW_USHORT dqt_size = 0;
	FW_USHORT dht_size = 0;
	FW_USHORT dri_size = 0;

	*sof_offset = 0;

	if (*buffer == 0xFF) {
		if (*(buffer + 1) == 0xC0) {
			// find SOF marker
			*sof_offset = 0;
		}
		else {
			// find other marker
			// get maker size
			dqt_size = dcf_exif_get_short_value_big_endian(buffer + 2);

			if (*(buffer + 2 + dqt_size) == 0xFF) {
				if (*(buffer + 2 + dqt_size + 1) == 0xC0) {
					// find SOF marker
					*sof_offset = 2 + dqt_size;
				}
				else {
					// find other marker
					// get maker size
					dht_size = dcf_exif_get_short_value_big_endian(buffer + 2 + dqt_size + 2);

					if (*(buffer + 2 + dqt_size + 2 + dht_size) == 0xFF) {
						if (*(buffer + 2 + dqt_size + 2 + dht_size + 1) == 0xC0) {
							// find SOF marker
							*sof_offset = 2 + dqt_size + 2 + dht_size;
						}
						else {
							// find other marker
							// get other size
							dri_size = dcf_exif_get_short_value_big_endian(buffer + 2 + dqt_size + 2 + dht_size + 2);
							if (*(buffer + 2 + dqt_size + 2 + dht_size + 2 + dri_size) == 0xFF) {
								if (*(buffer + 2 + dqt_size + 2 + dht_size + 2 + dri_size + 1) == 0xC0) {
									// find SOF marker
									*sof_offset = 2 + dqt_size + 2 + dht_size + 2 + dri_size;
								}
								else {
									*sof_offset = 2 + dqt_size + 2 + dht_size + 2 + dri_size + 6;
								}
							}
						}
					}
				}
			}
		}
	}
	return;
}


/**
 *	@brief		EXIF Tag type convert
 *	@param		(i) FW_UCHAR tag_type_val
 *	@param		(o) FW_UCHAR* tag_type_byte
 *	@return		
 *	@note		None
 *	@attention	None
 */
FW_VOID dcf_exif_tag_type_convert_byte(FW_UCHAR tag_type_val, FW_UCHAR* tag_type_byte)
{
	switch (tag_type_val) {
		case 1:
		case 2:
			// BYTE, ASCII
			*(tag_type_byte) = 1;
			break;

		case 3:
			// SHORT
			*(tag_type_byte) = 2;
			break;

		case 4:
			// LONG
			*(tag_type_byte) = 4;
			break;

		case 5:
			// RATIONAL
			*(tag_type_byte) = 8;
			break;

		case 7:
			// UNDEFINED
			*(tag_type_byte) = 1;
			break;

		case 9:
			// SLONG
			*(tag_type_byte) = 4;
			break;

		case 10:
			// SRATIONAL
			*(tag_type_byte) = 8;
			break;

		default:
			*(tag_type_byte) = 1;
			break;
	}
}


#ifdef CO_BF_FS_USER_CUSTOM_USE_BCD_DATE
/**
 * @brief set date and time(Use BCD Code).
 * @param o:*addr FW_CHAR array address
 * @param i:date_type value of date
 * @return FW_VOID
 * @note none
 * @attention none.
 */
static FW_VOID dcf_exif_set_bcd_datetime(FW_UCHAR* addr, T_BF_DCF_IF_DATE datetype)
{
	FW_UCHAR num;

	if (addr == NULL) {
		return;
	}

	// year
	num = datetype.year;
	*addr        = '2';
	*(addr + 1)  = '0';
	*(addr + 2)  = ((num & 0xF0) >> 4) + '0';
	*(addr + 3)  =  (num & 0x0F)       + '0';

	// month
	num = datetype.month;
	*(addr + 5)  = ((num & 0xF0) >> 4) + '0';
	*(addr + 6)  =  (num & 0x0F)       + '0';

	// day
	num = datetype.day;
	*(addr + 8)  = ((num & 0xF0) >> 4) + '0';
	*(addr + 9)  =  (num & 0x0F)       + '0';

	// hour
	num = datetype.hour;
	*(addr + 11) = ((num & 0xF0) >> 4) + '0';
	*(addr + 12) =  (num & 0x0F)       + '0';

	// minute
	num = datetype.min;
	*(addr + 14) = ((num & 0xF0) >> 4) + '0';
	*(addr + 15) =  (num & 0x0F)       + '0';

	// second
	num = datetype.sec;
	*(addr + 17) = ((num & 0xF0) >> 4) + '0';
	*(addr + 18) =  (num & 0x0F)       + '0';
}

#else
/**
 * @brief set date and time(Use Binary Code).
 * @param o:*addr FW_CHAR array address
 * @param i:date_type value of date
 * @return FW_VOID
 * @note none
 * @attention none.
 */
static FW_VOID dcf_exif_set_bin_datetime(FW_UCHAR* addr, T_BF_DCF_IF_DATE datetype)
{
	FW_USHORT num;

	if (addr == NULL) {
		return;
	}

	// year
	num = datetype.ad_year;
	*addr        =  (num / 1000)      + '0';
	*(addr + 1)  = ((num / 100) % 10) + '0';
	*(addr + 2)  = ((num / 10)  % 10) + '0';
	*(addr + 3)  =  (num        % 10) + '0';

	// month
	num = datetype.month;
	*(addr + 5)  = (num / 10) + '0';
	*(addr + 6)  = (num % 10) + '0';

	// day
	num = datetype.day;
	*(addr + 8)  = (num / 10) + '0';
	*(addr + 9)  = (num % 10) + '0';

	// hour
	num = datetype.hour;
	*(addr + 11) = (num / 10) + '0';
	*(addr + 12) = (num % 10) + '0';

	// minute
	num = datetype.min;
	*(addr + 14) = (num / 10) + '0';
	*(addr + 15) = (num % 10) + '0';

	// second
	num = datetype.sec;
	*(addr + 17) = (num / 10) + '0';
	*(addr + 18) = (num % 10) + '0';
}
#endif

/**
 * @brief check tag value type(value or offset)
 * @param i:type : tag type
 * @param i:count : count
 * @param o:*val_type
 *				D_BF_DCF_EXIF_TYPE_VALUE
 *				D_BF_DCF_EXIF_TYPE_OFFSET
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_exif_check_tag_value_type(FW_UCHAR type, FW_ULONG count, FW_CHAR* val_type)
{
	switch(type){
		case D_BF_DCF_EXIF_BYTE_T: // FALL THROUGH
		case D_BF_DCF_EXIF_ASCII_T: // FALL THROUGH
		case D_BF_DCF_EXIF_UNDEFINED_T:
			if (count <= 4) {
				// value
				*val_type = D_BF_DCF_EXIF_TYPE_VALUE;
			}
			else {
				// offset
				*val_type = D_BF_DCF_EXIF_TYPE_OFFSET;
			}
			break;

		case D_BF_DCF_EXIF_SHORT_T:
			if (count <= 2) {
				// value
				*val_type = D_BF_DCF_EXIF_TYPE_VALUE;
			}
			else {
				// offset
				*val_type = D_BF_DCF_EXIF_TYPE_OFFSET;
			}
			break;

		case D_BF_DCF_EXIF_LONG_T: // FALL THROUGH
		case D_BF_DCF_EXIF_RATIONAL_T: // FALL THROUGH
		case D_BF_DCF_EXIF_SLONG_T: // FALL THROUGH
		case D_BF_DCF_EXIF_SRATIONAL_T:
			// offset
			*val_type = D_BF_DCF_EXIF_TYPE_OFFSET;
			break;

		default:
			*val_type = 0;
			return FSDCF_ERRNO_PARAM_FAILED;
	}
	
	return FSDCF_ERRNO_OK;
}

#ifdef CO_BF_DCF_IF_GET_GPSTAG
/**
 * @brief get exif tag value GPS Latitude or Longitude.
 * @param i:file_id file id
 * @param i:*add target address
 * @param i/o:*value_offset pointer of tag value offset
 * @param i:*total_buf pointer of total buffer size
 * @param o:*value GPS Latitude or Longitude
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 dcf_exif_get_gps_coordinate_value(FW_INT32 file_id, FW_UCHAR* add, FW_ULONG* value_offset, FW_ULONG total_buf, T_BF_DCF_EXIF_GPS_LATITUDE_LONGITUDE* value)
{
	FW_INT32  ercd;
	FW_UCHAR* tmp_buff;
	FW_ULONG  result;
	FW_ULONG  surplus;
	FW_ULONG  before_total,after_total;
	FW_INT32  ptr = 0;
	FW_ULONG  tmp_offset;

	if (add == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (value_offset == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (value == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	before_total = total_buf;
	after_total = total_buf;

	if (*value_offset > total_buf) {
		ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&tmp_buff);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
			return ercd;
		}

		ercd = BF_Mdf_Wrapper_Lseek(file_id, 0, D_BF_MDF_WRAPPER_SEEK_CUR, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			return ercd;
		}

		for (;;) {
			ercd = BF_Mdf_Wrapper_Read(file_id, tmp_buff, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
			if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
				return ercd;
			}

			after_total += result;
			if (after_total > *value_offset) {
				*value_offset -= before_total;
				break;
			}
			else {
				before_total += result;
			}
		}

		// get dd Numerator
		value->DdNum = dcf_exif_get_long_value(tmp_buff + *value_offset);

		// get dd Denominator
		tmp_offset = *value_offset + 4;
		value->DdDen = dcf_exif_get_long_value(tmp_buff + tmp_offset);

		// get mm Numerator
		tmp_offset += 4;
		value->MmNum = dcf_exif_get_long_value(tmp_buff + tmp_offset);

		// get mm Denominator
		tmp_offset += 4;
		value->MmDen = dcf_exif_get_long_value(tmp_buff + tmp_offset);

		// get ss Numerator
		tmp_offset += 4;
		value->SsNum = dcf_exif_get_long_value(tmp_buff + tmp_offset);

		// get ss Denominator
		tmp_offset += 4;
		value->SsDen = dcf_exif_get_long_value(tmp_buff + tmp_offset);

		ercd = BF_Mdf_Wrapper_Lseek(file_id, ptr, D_BF_MDF_WRAPPER_SEEK_SET, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
			return ercd;
		}

		ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)tmp_buff);
		if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
			return ercd;
		}
	}
	else {
		surplus = (total_buf % D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		if (surplus == 0) {
			*value_offset -= (total_buf - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		else {
			*value_offset -= (total_buf - surplus);
		}
		// get dd Numerator
		value->DdNum = dcf_exif_get_long_value(add + *value_offset);

		// get dd Denominator
		tmp_offset = *value_offset + 4;
		value->DdDen = dcf_exif_get_long_value(add + tmp_offset);

		// get mm Numerator
		tmp_offset += 4;
		value->MmNum = dcf_exif_get_long_value(add + tmp_offset);

		// get mm Denominator
		tmp_offset += 4;
		value->MmDen = dcf_exif_get_long_value(add + tmp_offset);

		// get ss Numerator
		tmp_offset += 4;
		value->SsNum = dcf_exif_get_long_value(add + tmp_offset);

		// get ss Denominator
		tmp_offset += 4;
		value->SsDen = dcf_exif_get_long_value(add + tmp_offset);
	}

	return 0;
}
#endif	// CO_BF_DCF_IF_GET_GPSTAG

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
//----------------------------------------------------------------------------
// for MPF
//----------------------------------------------------------------------------

static FW_VOID dcf_exif_swap_long(FW_ULONG* data)
{
	FW_ULONG tmp_long_val;

	tmp_long_val = ( (((*data)&0xFF000000) >>24) | (((*data)&0x00FF0000) >>8) | (((*data)&0x0000FF00) <<8) | (((*data)&0x000000FF) <<24) );
	*data = tmp_long_val;
}

static FW_VOID dcf_exif_swap_short(FW_USHORT* data)
{
	FW_USHORT tmp_short_val;

	tmp_short_val = ( (((*data) & 0xFF00 ) >> 8) | (((*data) & 0x00FF) << 8) );
	*data = tmp_short_val;
}

static FW_VOID dcf_exif_exchange_endian_mpf(FW_UCHAR mpf_index)
{
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].tiff_head.ByteOrder = g_bf_dcf_exif_big_or_lit;

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].tiff_head.Reserved);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].tiff_head.Offset);

	// IFD0
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num);

	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Tag != 0) {
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Tag);
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Type);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Count);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.ValueOffset.Offset);

		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_value.ValueXResolution.Num);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_value.ValueXResolution.Den);
	}

	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Tag != 0) {
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Tag);
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Type);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Count);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.ValueOffset.Offset);

		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_value.ValueYResolution.Num);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_value.ValueYResolution.Den);
	}

	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.Tag != 0) {
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.Tag);
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.Type);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.Count);
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.ValueOffset.SValue.Value);
	}

	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.Tag != 0) {
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.Tag);
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.Type);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.Count);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.ValueOffset.LValue);
	}

	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.offset_to_1stIFD);

	// Exif IFD
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num);
	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Tag != 0) {
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Tag);
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Type);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Count);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.ValueOffset.Offset);
	}

	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Tag != 0) {
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Type == D_BF_DCF_EXIF_SHORT_T) {
			dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.ValueOffset.SValue.Value);
		}
		else {
			dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.ValueOffset.LValue);
		}
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Tag);
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Type);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Count);
	}

	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Tag != 0) {
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Type == D_BF_DCF_EXIF_SHORT_T) {
			dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.ValueOffset.SValue.Value);
		}
		else {
			dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.ValueOffset.LValue);
		}
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Tag);
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Type);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Count);
	}

	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Tag != 0) {
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Tag);
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Type);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Count);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.ValueOffset.Offset);
	}

	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.NextIFDOffset);

	// IFD1
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num);
	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.Tag != 0) {
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.Tag);
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.Type);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.Count);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.ValueOffset.LValue);
	}

	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormatLength.Tag != 0) {
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormatLength.Tag);
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormatLength.Type);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormatLength.Count);
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormatLength.ValueOffset.LValue);
	}
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.NextIFDOffset);
}

/**
 * @brief get ycbcr format.
 * @param i:file_id file id
 * @param i:buffer read buffer
 * @param i:read_offset offset
 * @param o:*ycbcr_format YCbCr format
 * @return error code
 * @note none
 * @attention none.
 */
static FW_INT32 dcf_exif_get_ycbcr_format(FW_INT32 file_id, FW_UCHAR *buffer, FW_ULONG read_offset, FW_UCHAR *ycbcr_format)
{
	FW_INT32  ercd;
	FW_UCHAR  *buffer_offset;
	FW_ULONG  total_length;
	FW_ULONG  feild_length;
	FW_ULONG  result;
	FW_INT32  save_endian;

	ercd = 0;
	total_length = 0;
	feild_length = 0;

	total_length = read_offset;
	buffer_offset = buffer;
	save_endian = g_bf_dcf_exif_big_or_lit;
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;

	while (*(buffer_offset) == 0xFF) {
		if ((*(buffer_offset + 1) == 0xD8) || (*(buffer_offset + 1) == 0xD9)) {
			// SOI,EOI
			buffer_offset = (buffer_offset + 2);
			total_length = total_length + 2;
		}
		else if ((*(buffer_offset + 1) == 0xE1) || (*(buffer_offset + 1) == 0xE2) || (*(buffer_offset + 1) == 0xEB) ||
				 (*(buffer_offset + 1) == 0xDB) || (*(buffer_offset + 1) == 0xC4) || (*(buffer_offset + 1) == 0xDD)) {
			// APP1,APP2,APP11,DQT,DHT,DRI
			feild_length = dcf_exif_get_short_value_big_endian((buffer_offset + 2));
			total_length = total_length + feild_length + 2;

			ercd = BF_Mdf_Wrapper_Lseek(file_id, total_length, SEEK_SET, &result);
			if (ercd == 0) {
				ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
				if (ercd == 0) {
					buffer_offset = buffer;
				}
				else {
					// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
					break;
				}
			}
			else {
				// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				break;
			}
		}
		else if (*(buffer_offset + 1) == 0xC0) {
			// SOF0
			// ycbcr format
			*ycbcr_format = *(buffer_offset + 0xB);
			break;
		}
		else {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND SOF0 MARKER\n"));
#endif
			break;
		}
	}

	g_bf_dcf_exif_big_or_lit = save_endian;

	return ercd;
}

#endif // CO_BF_DCF_IF_MPF_ENABLE

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @brief analyze exif tag offset in MPO.
 * @param i:file_id file id
 * @param o:*load_image_data image data info
 * @param o:*img_detail image detail information
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Exif_Analyze_Ext(FW_INT32 file_id, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data, T_BF_DCF_IF_IMAGE_DETAIL* img_detail)
{
	FW_INT32  ercd, ercd2;
	FW_UCHAR  *buffer;
	FW_ULONG  result;
	FW_USHORT app1_size = 0;
	FW_USHORT app2_size = 0;
	FW_ULONG  zero_ifd_num = 0;
	FW_ULONG  next_ifd_ptr = 0;
	FW_ULONG  first_ifd_offset = 0;
	FW_ULONG  tag_offset = 0;
	FW_ULONG  reset_offset = 0;
	FW_ULONG  value_offset = 0;
	FW_ULONG  total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
	FW_UCHAR  tag_type;
	FW_UCHAR  tag_type_byte;
	FW_ULONG  tag_count;
	FW_ULONG  data_offset;
	FW_USHORT tmp_value;
	FW_USHORT app_zero_size = 0;
	FW_USHORT comment_size = 0;
	FW_USHORT jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
	FW_USHORT zero_ifd_offset   = D_BF_DCF_EXIF_TIFFHEADSIZE;

	FW_ULONG  soh_offset = 0;
	FW_UCHAR  app2_flag  = 0;
	FW_ULONG  total_read = 0;
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	FW_ULONG  tag_offset_save;
	FW_ULONG  value_offset_save;
	FW_ULONG  reset_offset_save;
#endif
#ifdef CO_BF_DCF_IF_MPF_ENABLE
	FW_ULONG  mp_tag_offset = 0;
	FW_ULONG  mp_head_offset;
	FW_USHORT mp_entry_num;
	FW_USHORT mp_entry_cnt;
	T_BF_DCF_EXIF_MP_ENTRY mp_entry;
#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
	FW_USHORT ext_num = 0;
	FW_USHORT ext_cnt = 0;
	T_BF_DCF_EXIF_MP_ENTRY mp_child_entry;
	FW_ULONG  ext_start_offset = 0;
#endif    // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
#endif	// CO_BF_DCF_IF_MPF_ENABLE
	FW_CHAR tag_val_type;
#ifdef CO_BF_DCF_IF_GET_GPSTAG
	FW_ULONG  gps_ifd_offset = 0;
#endif	// CO_BF_DCF_IF_GET_GPSTAG

	if (load_image_data == NULL) {
		// parameter error
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (img_detail == NULL) {
		// parameter error
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	// data clear
#ifdef CO_BF_DCF_IF_MPF_ENABLE
	img_detail->baseline_mp_flag  = 0;
#endif	// CO_BF_DCF_IF_MPF_ENABLE

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
#endif

#ifdef CO_BF_DCF_IF_MPF_ENABLE
	load_image_data->main_size           = 0;
	load_image_data->class1_size         = 0;
	load_image_data->class1_offset       = 0;
	load_image_data->class1_thumb_size   = 0;
	load_image_data->class1_thumb_offset = 0;
	load_image_data->class2_size         = 0;
	load_image_data->class2_offset       = 0;
	load_image_data->class2_thumb_size   = 0;
	load_image_data->class2_thumb_offset = 0;
#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
	for (ext_cnt=0; ext_cnt<E_BF_DCF_IF_MPF_EXT_MP_SELECT_MAX; ext_cnt++) {
		load_image_data->ext_mp[ext_cnt].main_size         = 0;
		load_image_data->ext_mp[ext_cnt].main_offset       = 0;
		load_image_data->ext_mp[ext_cnt].thumb_size   = 0;
		load_image_data->ext_mp[ext_cnt].thumb_offset = 0;
		load_image_data->ext_mp[ext_cnt].class1_size         = 0;
		load_image_data->ext_mp[ext_cnt].class1_offset       = 0;
		load_image_data->ext_mp[ext_cnt].class1_thumb_size   = 0;
		load_image_data->ext_mp[ext_cnt].class1_thumb_offset = 0;
		load_image_data->ext_mp[ext_cnt].class2_size         = 0;
		load_image_data->ext_mp[ext_cnt].class2_offset       = 0;
		load_image_data->ext_mp[ext_cnt].class2_thumb_size   = 0;
		load_image_data->ext_mp[ext_cnt].class2_thumb_offset = 0;
		load_image_data->ext_mp_num = 0;
	}	
#endif    // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE	
#endif	// CO_BF_DCF_IF_MPF_ENABLE

	memset(&img_detail->exiftag_flg, 0x00, sizeof(FW_ULONG));

	// memory allocation
	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&buffer);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		// memory get error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	while (1) {
		// read data from Exif
		// file pointer : file top
		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
			// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		// check DQT marker
		if (*(buffer + 3) == 0xDB) {
			// detect JPEG(without APPx) format
			img_detail->jpeg_format = E_BF_DCF_IF_JPEG_FORMAT_NOT_EXIF;
			dcf_exif_exif_info_dummy(img_detail);
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;

		}
		// check APP0 marker
		else if (*(buffer + 3) == 0xE0) {
			// detect APP0
			app_zero_size = dcf_exif_get_short_value_big_endian(buffer + 4) + 2;
			jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET + app_zero_size;

			// check COM marker
			if (*(buffer + app_zero_size + 3) == 0xFE) {
				comment_size = dcf_exif_get_short_value_big_endian(buffer + app_zero_size + 4) + 2;
				app_zero_size = app_zero_size + comment_size;
			}
		}
		else {
			; // DO NOTHING
		}

		// check DQT marker
		if (*(buffer + app_zero_size + 3) == 0xDB) {
			// detect JFIF(no thumbnail) format
			img_detail->jpeg_format = E_BF_DCF_IF_JPEG_FORMAT_NOT_EXIF;
			dcf_exif_exif_info_dummy(img_detail);
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;

		}
		// check APP0 marker
		else if (*(buffer + app_zero_size + 3) == 0xE0) {
			// detect JFIF(JFXX) format
			img_detail->jpeg_format = E_BF_DCF_IF_JPEG_FORMAT_NOT_EXIF;
			dcf_exif_exif_info_dummy(img_detail);
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;

		}
		// check APP1 marker
		// APP1 marker's place is next to SOI marker
		else if (*(buffer + app_zero_size + 3) != 0xE1) {
			// not detect APP1
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND APP1 MARKER\n"));
#endif
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;
		}
		else {
			; // DO NOTHING
		}

		app1_size = dcf_exif_get_short_value_big_endian((buffer + app_zero_size + 4));

		// reset pointer
		ercd = BF_Mdf_Wrapper_Lseek(file_id, (4 + app_zero_size + app1_size), SEEK_SET, &result);
		if (ercd != 0) {
			// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
			// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}
		
		if (*(buffer + 1) == 0xE2) {
			// detect APP2
			app2_size = dcf_exif_get_short_value_big_endian(buffer + 2);
			app2_flag = 1;
			//total_read += (2 + app2_size);

#ifdef CO_BF_DCF_IF_MPF_ENABLE
			if (strcmp((const FW_CHAR*)(buffer + 4), "MPF") == 0) {
				// MPF format
				img_detail->baseline_mp_flag = 1;
				img_detail->extended_mp_flag    = 0;
				mp_head_offset = (4 + app1_size + 8);

				g_bf_dcf_exif_big_or_lit = *(FW_USHORT*)(buffer + 8);
				mp_tag_offset = dcf_exif_get_long_value(buffer + 12) + 10;
				gNumOfTag = dcf_exif_get_short_value(buffer + mp_tag_offset - 2);	// Num of MP index IFD's tag
				gTagCheckCnt = 1;	// reset seearch count

				// Check MP Entry Num
				ercd = dcf_exif_check_exif_tag(file_id, buffer, 0xB001, &mp_tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
				if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
					// MP Entry num not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND MP ENTRY NUM\n"));
#endif
					break;
				}
				else if (ercd != 0) {
					// error
					break;
				}
				else {
					// get MP entry num
					mp_entry_num = dcf_exif_get_long_value(buffer + mp_tag_offset + value_offset);
				}
				tag_offset = reset_offset;

				// Check MP Entry
				ercd = dcf_exif_check_exif_tag(file_id, buffer, 0xB002, &mp_tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
				if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
					// MP entry not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND MP ENTRY\n"));
#endif
					break;
				}
				else if (ercd != 0) {
					// error
					break;
				}
				else {
					// get MP entry
					data_offset = 8 + dcf_exif_get_long_value(buffer + mp_tag_offset + value_offset);

					ext_num = 0;
					for (mp_entry_cnt = 0; mp_entry_cnt < mp_entry_num; mp_entry_cnt++) {
							
						mp_entry.mp_info        = dcf_exif_get_long_value(buffer + data_offset + (16 * mp_entry_cnt));
						mp_entry.mp_data_size   = dcf_exif_get_long_value(buffer + data_offset + (16 * mp_entry_cnt) + 4);
						mp_entry.mp_data_offset = dcf_exif_get_long_value(buffer + data_offset + (16 * mp_entry_cnt) + 8);
						mp_entry.mp_entry_1     = dcf_exif_get_short_value(buffer + data_offset + (16 * mp_entry_cnt) + 12);
						mp_entry.mp_entry_2     = dcf_exif_get_short_value(buffer + data_offset + (16 * mp_entry_cnt) + 14);

						if ((mp_entry.mp_info & 0x000FFFFF) == 0x00030000) {
							// Representative Image
							load_image_data->main_size = mp_entry.mp_data_size;

							if (mp_entry.mp_entry_1 != 0) {
								mp_child_entry.mp_info  = dcf_exif_get_long_value(buffer + data_offset + (16 * (mp_entry.mp_entry_1 - 1)));
								mp_child_entry.mp_data_size   = dcf_exif_get_long_value(buffer + data_offset + (16 * (mp_entry.mp_entry_1 - 1)) + 4);
								mp_child_entry.mp_data_offset = dcf_exif_get_long_value(buffer + data_offset + (16 * (mp_entry.mp_entry_1 - 1)) + 8);
								if ((mp_child_entry.mp_info & 0x000FFFFF) == 0x00010001) {
									// ScreenNail (VGA)
									load_image_data->class1_size   = mp_child_entry.mp_data_size;
									load_image_data->class1_offset = mp_child_entry.mp_data_offset + mp_head_offset;
								}
							}
							if (mp_entry.mp_entry_2 != 0) {
								mp_child_entry.mp_info  = dcf_exif_get_long_value(buffer + data_offset + (16 * (mp_entry.mp_entry_2 - 1)));
								mp_child_entry.mp_data_size   = dcf_exif_get_long_value(buffer + data_offset + (16 * (mp_entry.mp_entry_2 - 1)) + 4);
								mp_child_entry.mp_data_offset = dcf_exif_get_long_value(buffer + data_offset + (16 * (mp_entry.mp_entry_2 -1 )) + 8);
								if ((mp_child_entry.mp_info & 0x000FFFFF) == 0x00010002) {
									// ScreenNail (Full HD)
									load_image_data->class2_size   = mp_child_entry.mp_data_size;
									load_image_data->class2_offset = mp_child_entry.mp_data_offset + mp_head_offset;
								}
							}
							ext_num++;
						}
						else if ((mp_entry.mp_info & 0x000FFFFF) == 0x00020002) {							
							load_image_data->ext_mp[ext_num].main_size = mp_entry.mp_data_size;
							load_image_data->ext_mp[ext_num].main_offset= mp_entry.mp_data_offset + mp_head_offset;

							if (mp_entry.mp_entry_1 != 0) {
								mp_child_entry.mp_info  = dcf_exif_get_long_value(buffer + data_offset + (16 * (mp_entry.mp_entry_1 - 1)));
								mp_child_entry.mp_data_size   = dcf_exif_get_long_value(buffer + data_offset + (16 * (mp_entry.mp_entry_1 - 1)) + 4);
								mp_child_entry.mp_data_offset = dcf_exif_get_long_value(buffer + data_offset + (16 * (mp_entry.mp_entry_1 - 1)) + 8);
								if ((mp_child_entry.mp_info & 0x000FFFFF) == 0x00010001) {
									// ScreenNail (VGA)
									load_image_data->ext_mp[ext_num].class1_size   = mp_child_entry.mp_data_size;
									load_image_data->ext_mp[ext_num].class1_offset = mp_child_entry.mp_data_offset + mp_head_offset;
								}
								else if ((mp_child_entry.mp_info & 0x000FFFFF) == 0x00010002) {
									// ScreenNail (Full HD)
									load_image_data->ext_mp[ext_num].class2_size   = mp_child_entry.mp_data_size;
									load_image_data->ext_mp[ext_num].class2_offset = mp_child_entry.mp_data_offset + mp_head_offset;
								}
							}
							if (mp_entry.mp_entry_2 != 0) {
								mp_child_entry.mp_info  = dcf_exif_get_long_value(buffer + data_offset + (16 * (mp_entry.mp_entry_2 - 1)));
								mp_child_entry.mp_data_size   = dcf_exif_get_long_value(buffer + data_offset + (16 * (mp_entry.mp_entry_2 - 1)) + 4);
								mp_child_entry.mp_data_offset = dcf_exif_get_long_value(buffer + data_offset + (16 * (mp_entry.mp_entry_2 - 1)) + 8);
								
								if ((mp_child_entry.mp_info & 0x000FFFFF) == 0x00010001) {
									// ScreenNail (VGA)
									load_image_data->ext_mp[ext_num].class1_size   = mp_child_entry.mp_data_size;
									load_image_data->ext_mp[ext_num].class1_offset = mp_child_entry.mp_data_offset + mp_head_offset;
								}
								else if ((mp_child_entry.mp_info & 0x000FFFFF) == 0x00010002) {
									// ScreenNail (Full HD)
									load_image_data->ext_mp[ext_num].class2_size   = mp_child_entry.mp_data_size;
									load_image_data->ext_mp[ext_num].class2_offset = mp_child_entry.mp_data_offset + mp_head_offset;
								}
							}
							if (ext_num == 0) {
								load_image_data->main_size = load_image_data->ext_mp[0].main_size;
								load_image_data->class1_size = load_image_data->ext_mp[0].class1_size;
								load_image_data->class1_offset = load_image_data->ext_mp[0].class1_offset;
								load_image_data->class2_size = load_image_data->ext_mp[0].class2_size;
								load_image_data->class2_offset = load_image_data->ext_mp[0].class2_offset;
							}
							
							ext_num++;
							
							img_detail->baseline_mp_flag = 0;
							img_detail->extended_mp_flag = 1;
							img_detail->extended_mp_type = 0x0002;
						}
					}

					if (img_detail->extended_mp_flag) {
#if 0
						load_image_data->ext_mp[0].main_size = load_image_data->main_size;
						load_image_data->ext_mp[0].class1_size = load_image_data->class1_size;
						load_image_data->ext_mp[0].class1_offset = load_image_data->class1_offset;
						load_image_data->ext_mp[0].class2_size = load_image_data->class2_size;
						load_image_data->ext_mp[0].class2_offset = load_image_data->class2_offset;
#endif
						load_image_data->ext_mp_num = ext_num;
					}
				}
	
				g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
			}
			else {
				; // DO NOTHING
			}
#endif	// CO_BF_DCF_IF_MPF_ENABLE
		}

		if (app2_flag == 1) {
			// reset pointer
			ercd = BF_Mdf_Wrapper_Lseek(file_id, (4 + app_zero_size + app1_size + 2 + app2_size), SEEK_SET, &result);
			if (ercd != 0) {
				// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				break;
			}

			ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
			if (ercd != 0) {
				// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				break;
			}
		}

		while ((*(buffer + 1) != 0xDB) && (*(buffer + 1) != 0xC0) && (*(buffer + 1) != 0xC4)) {
			if (*(buffer) != 0xFF) {
				// not find SOF marker
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND SOF MARKER\n"));
#endif
				ercd = FSDCF_ERRNO_NOT_APP1;
				break;
			}
			total_read += dcf_exif_get_short_value_big_endian(buffer + 2) + 2;

			if (app2_flag == 1) {
				// APP2 exist
				// reset pointer
				ercd = BF_Mdf_Wrapper_Lseek(file_id, (4 + app_zero_size + app1_size + 2 + app2_size + total_read), SEEK_SET, &result);
				if (ercd != 0) {
					// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
					break;
				}
			}
			else {
				// APP2 not exist
				// reset pointer
				ercd = BF_Mdf_Wrapper_Lseek(file_id, (4 + app_zero_size + app1_size + total_read), SEEK_SET, &result);
				if (ercd != 0) {
					// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
					break;
				}
			}

			ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
			if (ercd != 0) {
				// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				break;
			}
		}

		//soh_offset = total_read;
		// get SOF offset of buffer
		dcf_exif_get_sof_offset(buffer, &soh_offset);

		// Horizontal lines(7Byte from SOF top)
		img_detail->main_width = dcf_exif_get_short_value_big_endian(buffer + soh_offset + 0x7);
		// Vertical lines(5Byte from SOF top)
		img_detail->main_lines = dcf_exif_get_short_value_big_endian(buffer + soh_offset + 0x5);

		// ycbcr format
		img_detail->ycc_format = *(buffer + soh_offset + 0xB);

		// detect EXIF format
		img_detail->jpeg_format = E_BF_DCF_IF_JPEG_FORMAT_EXIF;

		// reset pointer
		ercd = BF_Mdf_Wrapper_Lseek(file_id, 0, SEEK_SET, &result);
		if (ercd != 0) {
			// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
			// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		// check endian
		g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));
		img_detail->byte_order = g_bf_dcf_exif_big_or_lit;

		zero_ifd_offset  = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);
		zero_ifd_num     = dcf_exif_get_short_value(buffer + jpeg_start_offset + zero_ifd_offset);
		next_ifd_ptr     = jpeg_start_offset + zero_ifd_offset + 2 + zero_ifd_num * sizeof(T_BF_DCF_EXIF_IFD);	/* point Next IFD Pointer */
		first_ifd_offset = dcf_exif_get_long_value(buffer + next_ifd_ptr);

		tag_offset   = jpeg_start_offset + zero_ifd_offset + 2;
		reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
		gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
		gTagCheckCnt = 1;	// reset seearch count

		// Get Maker
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_MAKE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND MAKE \n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Maker
			img_detail->exiftag_flg |= FJ_EXIF_TAG_MAKE_FLAG;
			dcf_exif_check_tag_value_type(tag_type, tag_count, &tag_val_type);
			if(tag_val_type == D_BF_DCF_EXIF_TYPE_VALUE){
				dcf_exif_tag_type_convert_byte(tag_type, &tag_type_byte);
				memcpy(&img_detail->maker, (void*)(buffer + tag_offset + value_offset), (tag_type_byte * tag_count));
			}
			else{
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				memcpy(&img_detail->maker, (void*)(buffer + data_offset), D_BF_DCF_EXIF_V_MAKE_S - 1);
			}
		}
		tag_offset = reset_offset;

		// Get Model
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_MODEL_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND MODEL \n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Model
			img_detail->exiftag_flg |= FJ_EXIF_TAG_MODEL_FLAG;
			dcf_exif_check_tag_value_type(tag_type, tag_count, &tag_val_type);
			if(tag_val_type == D_BF_DCF_EXIF_TYPE_VALUE){
				dcf_exif_tag_type_convert_byte(tag_type, &tag_type_byte);
				memcpy(&img_detail->model, (void*)(buffer + tag_offset + value_offset), (tag_type_byte * tag_count));
			}
			else{
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				memcpy(&img_detail->model, (void*)(buffer + data_offset), D_BF_DCF_EXIF_V_MODEL_S - 1);
			}
		}
		tag_offset = reset_offset;

		// Get Orientation
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_ORIENT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND ORIENTATION\n"));
#endif
			//img_detail->rotate = E_BF_DCF_IF_ROTATE_0;
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Orientation
			img_detail->exiftag_flg |= FJ_EXIF_TAG_ORIENT_FLAG;
			tmp_value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			//if(tmp_value < 9) {
			//	img_detail->rotate = (E_BF_DCF_IF_ROTATE)tmp_value;
			//}
			//else {
			//	img_detail->rotate = E_BF_DCF_IF_ROTATE_0;
			//}
			img_detail->rotate = (E_BF_DCF_IF_ROTATE)tmp_value;
		}
		tag_offset = reset_offset;

		// Get date & time
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_DATETIME_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EDIT DATE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get date & time
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_date_value(file_id, buffer, &data_offset, total_buffer, &img_detail->edit_date_time);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_DATETIME_FLAG;
		}
		tag_offset = reset_offset;

		// Get EXIF IDF offset
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND EXIF IDF OFFSET\n"));
#endif
			break;
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// set exif tag flag
			img_detail->exiftag_flg |= FJ_EXIF_TAG_EXIFPTR_FLAG;
		}
#ifdef CO_BF_DCF_IF_GET_GPSTAG
		tag_offset_save   = tag_offset;
		value_offset_save = value_offset;
		reset_offset_save = reset_offset;

		tag_offset = reset_offset;

		// Get GPS IDF offset
		ercd = dcf_exif_check_exif_tag(file_id, buffer, 0x8825, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("BF_Dcf_Exif_Analyze_Ext() CAN NOT FIND GPS IFD OFFSET\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			gps_ifd_offset = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
		}

		tag_offset   = tag_offset_save;
		value_offset = value_offset_save;
		reset_offset = reset_offset_save;
#endif	// CO_BF_DCF_IF_GET_GPSTAG

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		/* data save */
		tag_offset_save   = tag_offset;
		value_offset_save = value_offset;
		reset_offset_save = reset_offset;

		tag_offset = reset_offset;

		// Get PrintIM
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_PRINTIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PRINTIM\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get PrintIM
			img_detail->exiftag_flg |= FJ_EXIF_TAG_PRINTIM_FLAG;
			dcf_exif_check_tag_value_type(tag_type, tag_count, &tag_val_type);
			if(tag_val_type == D_BF_DCF_EXIF_TYPE_VALUE){
				dcf_exif_tag_type_convert_byte(tag_type, &tag_type_byte);
				memcpy(&img_detail->PrintIM, (void*)(buffer + tag_offset + value_offset), (tag_type_byte * tag_count));
			}
			else{
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				memcpy( &img_detail->PrintIM, (void*)(buffer + data_offset), D_BF_DCF_EXIF_V_PRINTIM_S - 1);
			}
		}
		// data restore
		tag_offset   = tag_offset_save;
		value_offset = value_offset_save;
		reset_offset = reset_offset_save;
#endif

		data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

		// get next start offset
		ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
		if (ercd != 0) {
			// error
			break;
		}

		// ExposureTime
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_EXPTIME_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EXPOSURE TIME\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Exposure Time
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, &img_detail->EpTimeNum, &img_detail->EpTimeDen);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_EXPTIME_FLAG;
		}
		tag_offset = reset_offset;

		// F Value
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_FNUM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND F_VALUE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get F Value
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, &img_detail->FvNum, &img_detail->FvDen);
			if (ercd != 0) {
				// gat value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_FNUM_FLAG;
		}
		tag_offset = reset_offset;

		// ExposureProgram
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_EXPPROG_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EXPOSURE_PROGRAM\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Exposure Program
			img_detail->exiftag_flg |= FJ_EXIF_TAG_EXPPROG_FLAG;
			img_detail->EpProgram = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
		tag_offset = reset_offset;

		// ISO speed ratings
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_ISOSPEEDRATE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND ISO SPEED RATINGS\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get ISO speed ratings
			img_detail->exiftag_flg |= FJ_EXIF_TAG_ISOSPEEDRATE_FLAG;
			//img_detail->iso = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			dcf_exif_tag_type_convert_byte(tag_type, &tag_type_byte);
			if ((tag_type_byte * tag_count) > 4) {
				// data size over 4byte
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				img_detail->iso = dcf_exif_get_short_value(buffer + data_offset);
			}
			else {
				// data size under 4byte
				img_detail->iso = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			}
		}
		tag_offset = reset_offset;

		// Get date & time Original
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_DATETIMEORIG_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND DATE TIME ORIGINAL\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get date & time Original
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_date_value(file_id, buffer, &data_offset, total_buffer, &img_detail->DateTimeOriginal);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_DATETIMEORIG_FLAG;
		}
		tag_offset = reset_offset;

		// Get date & time Digital
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_DATETIMEDITI_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND DATE TIME DIGITAL\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get date & time Digital
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_date_value(file_id, buffer, &data_offset, total_buffer, &img_detail->DateTimeDigital);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_DATETIMEDITI_FLAG;
		}
		tag_offset = reset_offset;

		// CompressedBitParPixel
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_COMPRESSBPP_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND BIT PAR PIXEL\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Compressed Bit Per Pixel
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, &img_detail->bitperpixelNum, &img_detail->bitperpixelDen);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_COMPRESSBPP_FLAG;
		}
		tag_offset = reset_offset;

		// ShutterSpeedValue
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_SHUTTER_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND SHUTTER SPEED\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get ShutterSpeedValue
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, &img_detail->ShutterSpeedNum, &img_detail->ShutterSpeedDen);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_SHUTTERSPEED_FLAG;
		}
		tag_offset = reset_offset;

		// Exposure bias
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_EXPBIAS_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND F_VALUE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Exposure bias
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
#if 0
			ercd = dcf_exif_get_exif_set_value_ex(file_id, buffer, &data_offset, total_buffer, &img_detail->ExposureBiasNum, &img_detail->ExposureBiasDen);
#endif
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->ExposureBiasNum, (FW_ULONG*)&img_detail->ExposureBiasDen);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_EXPBIAS_FLAG;
		}
		tag_offset = reset_offset;

		// MeteringMode
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_METERMODE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND METERING_MODE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Metering Mode
			img_detail->exiftag_flg |= FJ_EXIF_TAG_METERMODE_FLAG;
			img_detail->MeteringMode = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
		tag_offset = reset_offset;

		// LightSource
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_LIGHTSOURCE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND LIGHT_SOURCE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Light Source
			img_detail->exiftag_flg |= FJ_EXIF_TAG_LIGHTSOURCE_FLAG;
			img_detail->LightSource = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
		tag_offset = reset_offset;

		// Flash
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_FLASH_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND FLASH\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Flash
			img_detail->exiftag_flg |= FJ_EXIF_TAG_FLASH_FLAG;
			img_detail->flash = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
		tag_offset = reset_offset;

		// Maker Note
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_MAKERNOTE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND MAKERNOTE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Maker Note (offset & length)
			img_detail->exiftag_flg |= FJ_EXIF_TAG_MAKERNOTE_FLAG;
			img_detail->maker_note_offset = (FW_ULONG)(D_BF_DCF_EXIF_TIFFOFFSET + dcf_exif_get_long_value(buffer + tag_offset + value_offset));
			img_detail->maker_note_length = tag_count;
		}
		tag_offset = reset_offset;

		// Image Width */
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELXDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_X_DIMENSION\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Image Width
			img_detail->exiftag_flg |= FJ_EXIF_TAG_PEXELXDIM_FLAG;
			if (tag_type == 3) {
				// Short data
				img_detail->width = (FW_ULONG)dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			}
			else {
				// Long data
				img_detail->width = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			}
		}
		tag_offset = reset_offset;

		// Image Height
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELYDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_Y_DIMENSION\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Image Height
			img_detail->exiftag_flg |= FJ_EXIF_TAG_PEXELYDIM_FLAG;
			if (tag_type == 3) {
				// Short data
				img_detail->lines = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			}
			else {
				// Long data
				img_detail->lines = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			}
		}

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		// Custom Image Processing
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_CUSTOMRENDERED_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND CUSTOM RENDERED\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Custom Image Processing
			img_detail->exiftag_flg |= FJ_EXIF_TAG_CUSTOMRENDERED_FLAG;
			img_detail->CustomRendered = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
#endif

		// WhiteBalanceMode
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_WB_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND WHITE_BALANCE_MODE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get White Balance Mode
			img_detail->exiftag_flg |= FJ_EXIF_TAG_WB_FLAG;
			img_detail->WBMode = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
		tag_offset = reset_offset;

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		// Contrast
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_CONTRAST_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND CONTRAST\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Contrast
			img_detail->exiftag_flg |= FJ_EXIF_TAG_CONTRAST_FLAG;
			img_detail->Contrast = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}

		// Saturation
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_SATURATION_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND SATURATION\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Saturation
			img_detail->exiftag_flg |= FJ_EXIF_TAG_SATURATION_FLAG;
			img_detail->Saturation = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
#endif

#ifdef CO_BF_DCF_IF_GET_GPSTAG
		img_detail->gpstag_flg = 0;
		if (gps_ifd_offset != 0) {
			ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + gps_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
			if (ercd != 0) {
				// error
				break;
			}

			// Get GPSLatitudeRef
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_GPS_TAG_GPSLATITUDEREF_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND GPSLatitudeRef \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get GPSLatitudeRef
				img_detail->gpstag_flg |= FJ_GPS_TAG_GPSLATITUDEREF_FLAG;
				memcpy(&img_detail->gpsLatitudeRef, (void*)(buffer + tag_offset + value_offset), 2);
			}
			tag_offset = reset_offset;

			// Get GPSLatitude
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_GPS_TAG_GPSLATITUDE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND GPSLatitude \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get GPSLatitude
				T_BF_DCF_EXIF_GPS_LATITUDE_LONGITUDE coordinate_value;

				img_detail->gpstag_flg |= FJ_GPS_TAG_GPSLATITUDE_FLAG;
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
#if 1
				ercd = dcf_exif_get_gps_coordinate_value(file_id, buffer, &data_offset, total_buffer, &coordinate_value);
				img_detail->gpsLatitudeDdNum = coordinate_value.DdNum;
				img_detail->gpsLatitudeDdDen = coordinate_value.DdDen;
				img_detail->gpsLatitudeMmNum = coordinate_value.MmNum;
				img_detail->gpsLatitudeMmDen = coordinate_value.MmDen;
				img_detail->gpsLatitudeSsNum = coordinate_value.SsNum;
				img_detail->gpsLatitudeSsDen = coordinate_value.SsDen;

#else
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsLatitudeDdNum, (FW_ULONG*)&img_detail->gpsLatitudeDdDen);
				data_offset += 8;
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsLatitudeMmNum, (FW_ULONG*)&img_detail->gpsLatitudeMmDen);
				data_offset += 8;
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsLatitudeSsNum, (FW_ULONG*)&img_detail->gpsLatitudeSsDen);
#endif
			}
			tag_offset = reset_offset;

			// Get gpsLongitudeRef
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_GPS_TAG_GPSLONGITUDEREF_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND gpsLongitudeRef \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get gpsLongitudeRef
				img_detail->gpstag_flg |= FJ_GPS_TAG_GPSLONGITUDEREF_FLAG;
				memcpy(&img_detail->gpsLongitudeRef, (void*)(buffer + tag_offset + value_offset), 2);
			}
			tag_offset = reset_offset;

			// Get gpsLongitude
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_GPS_TAG_GPSLONGITUDE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND gpsLongitude \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get gpsLongitude
				T_BF_DCF_EXIF_GPS_LATITUDE_LONGITUDE coordinate_value;

				img_detail->gpstag_flg |= FJ_GPS_TAG_GPSLONGITUDE_FLAG;
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

#if 1
				ercd = dcf_exif_get_gps_coordinate_value(file_id, buffer, &data_offset, total_buffer, &coordinate_value);
				img_detail->gpsLongitudeDdNum = coordinate_value.DdNum;
				img_detail->gpsLongitudeDdDen = coordinate_value.DdDen;
				img_detail->gpsLongitudeMmNum = coordinate_value.MmNum;
				img_detail->gpsLongitudeMmDen = coordinate_value.MmDen;
				img_detail->gpsLongitudeSsNum = coordinate_value.SsNum;
				img_detail->gpsLongitudeSsDen = coordinate_value.SsDen;
#else
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsLongitudeDdNum, (FW_ULONG*)&img_detail->gpsLongitudeDdDen);
				data_offset += 8;
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsLongitudeMmNum, (FW_ULONG*)&img_detail->gpsLongitudeMmDen);
				data_offset += 8;
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsLongitudeSsNum, (FW_ULONG*)&img_detail->gpsLongitudeSsDen);
#endif
			}
			tag_offset = reset_offset;

			// Get gpsImgDirectionRef
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_GPS_TAG_GPSIMGDIRECTIONREF_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND gpsImgDirectionRef \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get gpsImgDirectionRef
				img_detail->gpstag_flg |= FJ_GPS_TAG_GPSIMGDIRECTIONREF_FLAG;
				memcpy(&img_detail->gpsImgDirectionRef, (void*)(buffer + tag_offset + value_offset), 2);
			}
			tag_offset = reset_offset;

			// Get gpsImgDirection
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_GPS_TAG_GPSIMGDIRECTION_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND gpsImgDirection \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get gpsImgDirection
				img_detail->gpstag_flg |= FJ_GPS_TAG_GPSIMGDIRECTION_FLAG;
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsImgDirectionNum, (FW_ULONG*)&img_detail->gpsImgDirectionDen);
			}
			tag_offset = reset_offset;
		}
#endif	// CO_BF_DCF_IF_GET_GPSTAG

		// get NEXT IFD Pointer
		ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + first_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
		if (ercd != 0) {
			// error
			break;
		}

		if( (tag_offset - 0xE) != 0) {
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
			// Get Maker1
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_MAKER1_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
				// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND MAKER1 \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get Maker (IFD1)
				img_detail->exiftag_flg |= FJ_EXIF_TAG_MAKER1_FLAG;
				dcf_exif_check_tag_value_type(tag_type, tag_count, &tag_val_type);
				if(tag_val_type == D_BF_DCF_EXIF_TYPE_VALUE){
					dcf_exif_tag_type_convert_byte(tag_type, &tag_type_byte);
					memcpy(&img_detail->maker1, (void*)(buffer + tag_offset + value_offset), (tag_type_byte * tag_count));
				}
				else{
					data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
					memcpy(&img_detail->maker1, (void*)(buffer + data_offset), D_BF_DCF_EXIF_V_MAKE_S - 1);
				}
			}
			tag_offset = reset_offset;

			// Get Model1
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_MODEL1_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
				// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND MODEL1\n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get Model (IFD1)
				img_detail->exiftag_flg |= FJ_EXIF_TAG_MODEL1_FLAG;
				dcf_exif_check_tag_value_type(tag_type, tag_count, &tag_val_type);
				if(tag_val_type == D_BF_DCF_EXIF_TYPE_VALUE){
					dcf_exif_tag_type_convert_byte(tag_type, &tag_type_byte);
					memcpy(&img_detail->model1, (void*)(buffer + tag_offset + value_offset), (tag_type_byte * tag_count));
				}
				else{
					data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
					memcpy(&img_detail->model1, (void*)(buffer + data_offset), D_BF_DCF_EXIF_V_MODEL_S - 1);
				}
			}
			tag_offset = reset_offset;
#endif

			// Get Orientation
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_ORIENT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
				// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND ORIENTATION\n"));
#endif
//			img_detail->rotate_thumb = E_BF_DCF_IF_ROTATE_0;
//			img_detail->rotate_thumb = img_detail->rotate;
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get Orientation (IFD1)
				img_detail->exiftag_flg |= FJ_EXIF_TAG_ORIENTTHM_FLAG;
				tmp_value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
				//if(tmp_value < 9) {
				//	img_detail->rotate_thumb = (E_BF_DCF_IF_ROTATE)tmp_value;
				//}
				//else {
				//	img_detail->rotate_thumb = E_BF_DCF_IF_ROTATE_0;
				//}
				img_detail->rotate_thumb = (E_BF_DCF_IF_ROTATE)tmp_value;
			}
			tag_offset = reset_offset;

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
			// Get date & time 1
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_DATETIME1_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
				// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EDIT DATE1\n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get date & time (IFD1)
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				ercd = dcf_exif_get_exif_date_value(file_id, buffer, &data_offset, total_buffer, &img_detail->date_time1);
				if (ercd != 0) {
					break;
				}
				img_detail->exiftag_flg |= FJ_EXIF_TAG_DATETIME1_FLAG;
			}
			tag_offset = reset_offset;
#endif

			// search JPEG InterchangeFormat IFD (0x201)
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
				// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND JPEG INTER_CHANGE_FORMAT\n"));
#endif
				break;
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get JPEG Interchange Format
				img_detail->exiftag_flg |= FJ_EXIF_TAG_JPG_INTERCHGFMT_FLAG;
			}
			load_image_data->thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

			// JPEG InterchangeFormatLength tag is 0x202
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
				// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND JPEG INTER_CHANGE_FORMAT_LENGTH\n"));
#endif
				break;
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get JPEG Interchange Format Length
				img_detail->exiftag_flg |= FJ_EXIF_TAG_JPG_INTERCHGFMTLEN_FLAG;
			}

			load_image_data->thumb_size = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = BF_Mdf_Wrapper_Lseek(file_id, load_image_data->thumb_offset, SEEK_SET, &result);
			if (ercd == 0) {
				FW_ULONG read_size = (load_image_data->thumb_size < D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE) ?
									 load_image_data->thumb_size : D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
				ercd = BF_Mdf_Wrapper_Read(file_id, buffer, read_size, &result);
				if (ercd == 0) {
					img_detail->ycc_format_thum = dcf_exif_get_thum_ycc_format(buffer, read_size);
				}
				else {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				}
			}
			else {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			}
		}
		else {
			// Not 1st IFD
			load_image_data->thumb_size = 0;
			load_image_data->thumb_offset = 0;
		}

#ifdef CO_BF_DCF_IF_MPF_ENABLE
		// YCBCR Format
		img_detail->class1_ycc_format = 0;
		img_detail->class2_ycc_format = 0;

		if (load_image_data->class1_size != 0) {
			// VGA ScreenNail Exist
			ercd = BF_Mdf_Wrapper_Lseek(file_id, load_image_data->class1_offset, SEEK_SET, &result);
			if (ercd == 0) {
				// lseek OK
				ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
				if (ercd == 0) {
					// read OK
					if ((*(buffer + 2) == 0xFF) && (*(buffer + 3) == 0xE1)) {
						// APP1 Exist
						jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
						g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));
						total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;

						zero_ifd_offset  = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);
						zero_ifd_num     = dcf_exif_get_short_value(buffer + jpeg_start_offset + zero_ifd_offset);
						next_ifd_ptr     = jpeg_start_offset + zero_ifd_offset + 2 + zero_ifd_num * sizeof(T_BF_DCF_EXIF_IFD);	/* point Next IFD Pointer */
						first_ifd_offset = dcf_exif_get_long_value(buffer + next_ifd_ptr);

						tag_offset   = jpeg_start_offset + zero_ifd_offset + 2;
						reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
						gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
						gTagCheckCnt = 1;	// reset seearch count

						/* Get EXIF IDF offset */
						ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
						if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
							// tag not find
							img_detail->class1_width = img_detail->width;
							img_detail->class1_lines = img_detail->lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
							BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EXIF IDF OFFSET SCREEN1\n"));
#endif
						}
						else if (ercd != 0) {
							// error
							break;
						}
						else {
							// get Exif IFD offset
							data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

							// get next start offset
							ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
							if (ercd != 0) {
								// error
								break;
							}

							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELXDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								img_detail->class1_width = img_detail->width;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
								BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_X_DIMENSION\n"));
#endif
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get Image Width
								if (tag_type == 3) {
									// Short data
									img_detail->class1_width = (FW_ULONG)dcf_exif_get_short_value(buffer + tag_offset + value_offset);
								}
								else {
									// Long data
									img_detail->class1_width = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
								}
							}
							tag_offset = reset_offset;

							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELYDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								img_detail->class1_lines = img_detail->lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
								BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_Y_DIMENSION\n"));
#endif
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get Image Height
								if (tag_type == 3) {
									// Short data
									img_detail->class1_lines = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
								}
								else {
									// Long data
									img_detail->class1_lines = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
								}
							}
						}

						if(first_ifd_offset != 0) {
							// get NEXT IFD Pointer
							ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + first_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
							if (ercd != 0) {
								// error
								break;
							}

							// search JPEG InterchangeFormat IFD (0x201)
							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								ercd = 0;
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get thumbnail offset
								load_image_data->class1_thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset) + load_image_data->class1_offset;
							}

							// JPEG InterchangeFormatLength tag is 0x202
							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								ercd = 0;
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get thumbnail size
								load_image_data->class1_thumb_size = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
							}
						}
					}
					else {
						// APP1 not exist
						img_detail->class1_width = img_detail->width;
						img_detail->class1_lines = img_detail->lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
						BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND APP1 in SCREEN1\n"));
#endif
					}

					// YCBCR Format
					ercd = dcf_exif_get_ycbcr_format(file_id, buffer, load_image_data->class1_offset, &img_detail->class1_ycc_format);
					if (ercd != 0) {
						// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
						BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND SOF0 in YCBCR FORMAT(class1)\n"));
#endif
					}

				}
				else {
					// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
					break;
				}
			}
			else {
				// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				break;
			}
		}
		else {
			// class1 not exist
			img_detail->class1_width = 0;
			img_detail->class1_lines = 0;
		}
		if (load_image_data->class2_size != 0) {
			// FullHD ScreenNail Exist
			ercd = BF_Mdf_Wrapper_Lseek(file_id, load_image_data->class2_offset, SEEK_SET, &result);
			if (ercd == 0) {
				// lseek OK
				ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
				if (ercd == 0) {
					// read OK
					if ((*(buffer + 2) == 0xFF) && (*(buffer + 3) == 0xE1)) {
						// APP1 Exist
						jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
						g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));
						total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;

						zero_ifd_offset  = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);
						zero_ifd_num     = dcf_exif_get_short_value(buffer + jpeg_start_offset + zero_ifd_offset);
						next_ifd_ptr     = jpeg_start_offset + zero_ifd_offset + 2 + zero_ifd_num * sizeof(T_BF_DCF_EXIF_IFD);	// point Next IFD Pointer
						first_ifd_offset = dcf_exif_get_long_value(buffer + next_ifd_ptr);

						tag_offset   = jpeg_start_offset + zero_ifd_offset + 2;
						reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
						gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
						gTagCheckCnt = 1;	// reset seearch count

						// Get EXIF IDF offset
						ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
						if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
							// tag not find
							img_detail->class2_width = img_detail->width;
							img_detail->class2_lines = img_detail->lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
							BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EXIF IDF OFFSET SCREEN2\n"));
#endif
						}
						else if (ercd != 0) {
							// error
							break;
						}
						else {
							// get Exif IFD offset
							data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

							// get next start offset
							ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
							if (ercd != 0) {
								// error
								break;
							}

							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELXDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								img_detail->class2_width = img_detail->width;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
								BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_X_DIMENSION SCREEN2\n"));
#endif
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get Image Width
								if (tag_type == 3) {
									// Short data
									img_detail->class2_width = (FW_ULONG)dcf_exif_get_short_value(buffer + tag_offset + value_offset);
								}
								else {
									// Long data
									img_detail->class2_width = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
								}
							}
							tag_offset = reset_offset;

							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELYDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								img_detail->class2_lines = img_detail->lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
								BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_Y_DIMENSION\n"));
#endif
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get Image Height
								if (tag_type == 3) {
									// Short data
									img_detail->class2_lines = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
								}
								else {
									// Long data
									img_detail->class2_lines = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
								}
							}
						}

						if(first_ifd_offset != 0) {
							// get NEXT IFD Pointer
							ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + first_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
							if (ercd != 0) {
								// error
								break;
							}

							// search JPEG InterchangeFormat IFD (0x201)
							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								ercd = 0;
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get thumbnail offset
								load_image_data->class2_thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset) + load_image_data->class2_offset;
							}

							// JPEG InterchangeFormatLength tag is 0x202
							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								ercd = 0;
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get thumbnail size
								load_image_data->class2_thumb_size = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
							}
						}
					}
					else {
						// APP1 not exist
						img_detail->class2_width = img_detail->width;
						img_detail->class2_lines = img_detail->lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
						BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND APP1 in SCREEN2\n"));
#endif
					}

					// YCBCR Format
					ercd = dcf_exif_get_ycbcr_format(file_id, buffer, load_image_data->class2_offset, &img_detail->class2_ycc_format);
					if (ercd != 0) {
						// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
						BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND SOF0 in YCBCR FORMAT(class2)\n"));
#endif
					}

				}
				else {
					// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
					break;
				}
			}
			else {
				// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				break;
			}
		}
		else {
			// class2 not exist
			img_detail->class2_width = 0;
			img_detail->class2_lines = 0;
		}
#endif     // CO_BF_DCF_IF_MPF_ENABLE
#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
		if (img_detail->extended_mp_flag != 0) {
			// save the first individual image info
			img_detail->ext_mp[0].main_width = img_detail->main_width;
			img_detail->ext_mp[0].main_lines= img_detail->main_lines;
			img_detail->ext_mp[0].ycc_format = img_detail->ycc_format;
			img_detail->ext_mp[0].ycc_format_thum = img_detail->ycc_format_thum;
						
			img_detail->ext_mp[0].class1_width = img_detail->class1_width;
			img_detail->ext_mp[0].class1_lines = img_detail->class1_lines;
			img_detail->ext_mp[0].class1_ycc_format = img_detail->class1_ycc_format;
			img_detail->ext_mp[0].class2_width = img_detail->class2_width;
			img_detail->ext_mp[0].class2_lines = img_detail->class2_lines;
			img_detail->ext_mp[0].class2_ycc_format = img_detail->class2_ycc_format;
			
			load_image_data->ext_mp[0].main_size = load_image_data->main_size;
			load_image_data->ext_mp[0].main_offset = load_image_data->main_offset;
			load_image_data->ext_mp[0].thumb_size = load_image_data->thumb_size;
			load_image_data->ext_mp[0].thumb_offset = load_image_data->thumb_offset;
			load_image_data->ext_mp[0].class1_size = load_image_data->class1_size;
			load_image_data->ext_mp[0].class1_offset = load_image_data->class1_offset;
			load_image_data->ext_mp[0].class1_thumb_offset = load_image_data->class1_thumb_offset;
			load_image_data->ext_mp[0].class1_thumb_size = load_image_data->class1_thumb_size;
			load_image_data->ext_mp[0].class2_size = load_image_data->class2_size;
			load_image_data->ext_mp[0].class2_offset = load_image_data->class2_offset;
			load_image_data->ext_mp[0].class2_thumb_offset = load_image_data->class2_thumb_offset;
			load_image_data->ext_mp[0].class2_thumb_size = load_image_data->class2_thumb_size;
			
			for (ext_cnt=1; ext_cnt<(FW_USHORT)load_image_data->ext_mp_num; ext_cnt++) {
				// YCBCR Format
				app_zero_size = 0;
				app2_size = 0;
				jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
				
				img_detail->ext_mp[ext_cnt].ycc_format = 0;
				img_detail->ext_mp[ext_cnt].ycc_format_thum = 0;

				if ((load_image_data->ext_mp[ext_cnt].main_size != 0) && (load_image_data->ext_mp[ext_cnt].main_offset != 0)) {
					ext_start_offset = load_image_data->ext_mp[ext_cnt].main_offset;
					// MPO Individual Exist
					ercd = BF_Mdf_Wrapper_Lseek(file_id, ext_start_offset, SEEK_SET, &result);
					if (ercd == 0) {
						// lseek OK
						ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
						if (ercd == 0) {
							// read OK
							if ((*(buffer + 2) == 0xFF) && (*(buffer + 3) == 0xE0)) {
								// detect APP0
								app_zero_size = dcf_exif_get_short_value_big_endian(buffer + 4) + 2;
								jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET + app_zero_size;
							}

							// check APP1 marker
							if ((*(buffer + app_zero_size + 2) == 0xFF) && (*(buffer + app_zero_size + 3) == 0xE1)) {
								// detect APP1
								app1_size = dcf_exif_get_short_value_big_endian((buffer + app_zero_size + 4));
							}

							// reset pointer
							ercd = BF_Mdf_Wrapper_Lseek(file_id, (ext_start_offset + 4 + app_zero_size + app1_size), SEEK_SET, &result);
							if (ercd != 0) {
								// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
								BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
								break;
							}

							ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
							if (ercd != 0) {
								// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
								BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
								break;
							}

							if (*(buffer + 1) == 0xE2) {
								// detect APP2
								app2_size = dcf_exif_get_short_value_big_endian(buffer + 2);

								// reset pointer
								ercd = BF_Mdf_Wrapper_Lseek(file_id, (ext_start_offset + 4 + app_zero_size + app1_size + 2 + app2_size), SEEK_SET, &result);
								if (ercd != 0) {
									// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
									BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
									break;
								}

								ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
								if (ercd != 0) {
									// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
									BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
									break;
								}
							}

							while ((*(buffer + 1) != 0xDB) && (*(buffer + 1) != 0xC0) && (*(buffer + 1) != 0xC4)) {
								if (*(buffer) != 0xFF) {
									// not find SOF marker
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
									BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND SOF MARKER\n"));
#endif
									continue;
								}
								total_read += dcf_exif_get_short_value_big_endian(buffer + 2) + 2;

								if (app2_size != 0) {
									// APP2 exist
									// reset pointer
									ercd = BF_Mdf_Wrapper_Lseek(file_id, (ext_start_offset + 4 + app_zero_size + app1_size + 2 + app2_size + total_read), SEEK_SET, &result);
									if (ercd != 0) {
										// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
										BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
										break;
									}
								}
								else {
									// APP2 not exist
									// reset pointer
									ercd = BF_Mdf_Wrapper_Lseek(file_id, (ext_start_offset + 4 + app_zero_size + app1_size + total_read), SEEK_SET, &result);
									if (ercd != 0) {
										// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
										BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
										break;
									}
								}

								ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
								if (ercd != 0) {
									// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
									BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
									break;
								}
							}
							
							// get SOF offset of buffer
							dcf_exif_get_sof_offset(buffer, &soh_offset);

							// Horizontal lines(7Byte from SOF top)
							img_detail->ext_mp[ext_cnt].main_width = dcf_exif_get_short_value_big_endian(buffer + soh_offset + 0x7);
							// Vertical lines(5Byte from SOF top)
							img_detail->ext_mp[ext_cnt].main_lines = dcf_exif_get_short_value_big_endian(buffer + soh_offset + 0x5);

							// ycbcr format
							img_detail->ext_mp[ext_cnt].ycc_format = *(buffer + soh_offset + 0xB);

							// reset pointer
							ercd = BF_Mdf_Wrapper_Lseek(file_id, ext_start_offset, SEEK_SET, &result);
							if (ercd != 0) {
								// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
								BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
								break;
							}
							ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
							if (ercd != 0) {
								// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
								BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
								break;
							}

							// check endian
							g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));

							zero_ifd_offset  = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);
							zero_ifd_num     = dcf_exif_get_short_value(buffer + jpeg_start_offset + zero_ifd_offset);
							next_ifd_ptr     = jpeg_start_offset + zero_ifd_offset + 2 + zero_ifd_num * sizeof(T_BF_DCF_EXIF_IFD);	/* point Next IFD Pointer */
							first_ifd_offset = dcf_exif_get_long_value(buffer + next_ifd_ptr);
							
							tag_offset   = jpeg_start_offset + zero_ifd_offset + 2;
							reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
							gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
							gTagCheckCnt = 1;	// reset seearch count
							
							/* Get EXIF IDF offset */
							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								img_detail->ext_mp[ext_cnt].main_width = img_detail->main_width;
								img_detail->ext_mp[ext_cnt].main_lines= img_detail->main_lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
								BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND EXIF IDF OFFSET\n"));
#endif
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get Exif IFD offset
								data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

								// get next start offset
								ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
								if (ercd != 0) {
									// error
									break;
								}

								ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELXDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
								if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
									// tag not find
									img_detail->ext_mp[ext_cnt].main_width = img_detail->main_width;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
									BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_X_DIMENSION\n"));
#endif
								}
								else if (ercd != 0) {
									// error
									break;
								}
								else {
									// get Image Width
									if (tag_type == 3) {
										// Short data
										img_detail->ext_mp[ext_cnt].main_width = (FW_ULONG)dcf_exif_get_short_value(buffer + tag_offset + value_offset);
									}
									else {
										// Long data
										img_detail->ext_mp[ext_cnt].main_width = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
									}
								}
								tag_offset = reset_offset;

								ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELYDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
								if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
									// tag not find
									img_detail->ext_mp[ext_cnt].main_lines = img_detail->main_lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
									BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_Y_DIMENSION\n"));
#endif
								}
								else if (ercd != 0) {
									// error
									break;
								}
								else {
									// get Image Height
									if (tag_type == 3) {
										// Short data
										img_detail->ext_mp[ext_cnt].main_lines = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
									}
									else {
										// Long data
										img_detail->ext_mp[ext_cnt].main_lines = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
									}
								}
								
								//data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
								
								// get NEXT IFD Pointer
								ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + first_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
								if (ercd != 0) {
									// error
									break;
								}

								if( (tag_offset - 0xE) != 0) {
									// search JPEG InterchangeFormat IFD (0x201)
									ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
									if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
										// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
										BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND EXT PICTURE INTER_CHANGE_FORMAT\n"));
#endif
										load_image_data->ext_mp[ext_cnt].thumb_offset = 0;
									}
									else if (ercd != 0) {
										// error
										break;
									}
									else {
 										load_image_data->ext_mp[ext_cnt].thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
									}
									
									// JPEG InterchangeFormatLength tag is 0x202
									ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
									if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
										// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
										BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND EXT INTER_CHANGE_FORMAT_LENGTH\n"));
#endif
										load_image_data->ext_mp[ext_cnt].thumb_size = 0;
									}
									else if (ercd != 0) {
										// error
										break;
									}
									else {
										load_image_data->ext_mp[ext_cnt].thumb_size = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
									}

									if (load_image_data->ext_mp[ext_cnt].thumb_size != 0 ) {
										ercd = BF_Mdf_Wrapper_Lseek(file_id, load_image_data->ext_mp[ext_cnt].thumb_offset, SEEK_SET, &result);
										if (ercd == 0) {
											FW_ULONG read_size = (load_image_data->ext_mp[ext_cnt].thumb_size < D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE) ?
																 load_image_data->ext_mp[ext_cnt].thumb_size : D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
											ercd = BF_Mdf_Wrapper_Read(file_id, buffer, read_size, &result);
											if (ercd == 0) {
												img_detail->ext_mp[ext_cnt].ycc_format_thum = dcf_exif_get_thum_ycc_format(buffer, read_size);
											}
											else {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
												BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
											}
										}
										else {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
											BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
										}
									}
								}
								else {
									// Not 1st IFD
									load_image_data->ext_mp[ext_cnt].thumb_offset = 0;
									load_image_data->ext_mp[ext_cnt].thumb_size = 0;
								}
								img_detail->ext_mp[ext_cnt].class1_ycc_format = 0;
								img_detail->ext_mp[ext_cnt].class1_ycc_format = 0;
								
								if (load_image_data->ext_mp[ext_cnt].class1_size != 0) {
									// VGA ScreenNail Exist
									ercd = BF_Mdf_Wrapper_Lseek(file_id, load_image_data->ext_mp[ext_cnt].class1_offset, SEEK_SET, &result);
									if (ercd == 0) {
										// lseek OK
										ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
										if (ercd == 0) {
											// read OK
											if ((*(buffer + 2) == 0xFF) && (*(buffer + 3) == 0xE1)) {
												// APP1 Exist
												jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
												g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));
												total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;

												zero_ifd_offset  = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);
												zero_ifd_num     = dcf_exif_get_short_value(buffer + jpeg_start_offset + zero_ifd_offset);
												next_ifd_ptr     = jpeg_start_offset + zero_ifd_offset + 2 + zero_ifd_num * sizeof(T_BF_DCF_EXIF_IFD);	/* point Next IFD Pointer */
												first_ifd_offset = dcf_exif_get_long_value(buffer + next_ifd_ptr);

												tag_offset   = jpeg_start_offset + zero_ifd_offset + 2;
												reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
												gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
												gTagCheckCnt = 1;	// reset seearch count

												/* Get EXIF IDF offset */
												ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
												if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
													// tag not find
													img_detail->ext_mp[ext_cnt].class1_width = img_detail->class1_width;
													img_detail->ext_mp[ext_cnt].class1_lines = img_detail->class1_lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
													BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EXIF IDF OFFSET SCREEN1\n"));
#endif
												}
												else if (ercd != 0) {
													// error
													break;
												}
												else {
													// get Exif IFD offset
													data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

													// get next start offset
													ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
													if (ercd != 0) {
														// error
														break;
													}

													ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELXDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
													if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
														// tag not find
														img_detail->ext_mp[ext_cnt].class1_width = img_detail->class1_width;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
														BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_X_DIMENSION\n"));
#endif
													}
													else if (ercd != 0) {
														// error
														break;
													}
													else {
														// get Image Width
														if (tag_type == 3) {
															// Short data
															img_detail->ext_mp[ext_cnt].class1_width = (FW_ULONG)dcf_exif_get_short_value(buffer + tag_offset + value_offset);
														}
														else {
															// Long data
															img_detail->ext_mp[ext_cnt].class1_width = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
														}
													}
													tag_offset = reset_offset;

													ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELYDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
													if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
														// tag not find
														img_detail->ext_mp[ext_cnt].class1_lines = img_detail->class1_lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
														BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_Y_DIMENSION\n"));
#endif
													}
													else if (ercd != 0) {
														// error
														break;
													}
													else {
														// get Image Height
														if (tag_type == 3) {
															// Short data
															img_detail->ext_mp[ext_cnt].class1_lines = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
														}
														else {
															// Long data
															img_detail->ext_mp[ext_cnt].class1_lines = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
														}
													}
												}

												if(first_ifd_offset != 0) {
													// get NEXT IFD Pointer
													ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + first_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
													if (ercd != 0) {
														// error
														break;
													}

													// search JPEG InterchangeFormat IFD (0x201)
													ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
													if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
														// tag not find
														ercd = 0;
													}
													else if (ercd != 0) {
														// error
														break;
													}
													else {
														// get thumbnail offset
														load_image_data->ext_mp[ext_cnt].class1_thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset) + load_image_data->ext_mp[ext_cnt].class1_offset;
													}

													// JPEG InterchangeFormatLength tag is 0x202
													ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
													if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
														// tag not find
														ercd = 0;
													}
													else if (ercd != 0) {
														// error
														break;
													}
													else {
														// get thumbnail size
														load_image_data->ext_mp[ext_cnt].class1_thumb_size = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
													}
												}
											}
											else {
												// APP1 not exist
												img_detail->ext_mp[ext_cnt].class1_width = img_detail->class1_width;
												img_detail->ext_mp[ext_cnt].class1_lines = img_detail->class1_lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
												BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND APP1 in EXT SCREEN1\n"));
#endif
											}

											// YCBCR Format
											ercd = dcf_exif_get_ycbcr_format(file_id, buffer, load_image_data->ext_mp[ext_cnt].class1_offset, &img_detail->ext_mp[ext_cnt].class1_ycc_format);
											if (ercd != 0) {
												// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
												BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND SOF0 in YCBCR FORMAT(class1)\n"));
#endif
											}

										}
										else {
											// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
											BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
											break;
										}
									}
									else {
										// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
										BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
										break;
									}
								}
								else {
									// class1 not exist
									img_detail->ext_mp[ext_cnt].class1_width = 0;
									img_detail->ext_mp[ext_cnt].class1_lines = 0;
								}
								if (load_image_data->ext_mp[ext_cnt].class2_size != 0) {
									// FullHD ScreenNail Exist
									ercd = BF_Mdf_Wrapper_Lseek(file_id, load_image_data->ext_mp[ext_cnt].class2_offset, SEEK_SET, &result);
									if (ercd == 0) {
										// lseek OK
										ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
										if (ercd == 0) {
											// read OK
											if ((*(buffer + 2) == 0xFF) && (*(buffer + 3) == 0xE1)) {
												// APP1 Exist
												jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
												g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));
												total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;

												zero_ifd_offset  = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);
												zero_ifd_num     = dcf_exif_get_short_value(buffer + jpeg_start_offset + zero_ifd_offset);
												next_ifd_ptr     = jpeg_start_offset + zero_ifd_offset + 2 + zero_ifd_num * sizeof(T_BF_DCF_EXIF_IFD);	// point Next IFD Pointer
												first_ifd_offset = dcf_exif_get_long_value(buffer + next_ifd_ptr);

												tag_offset   = jpeg_start_offset + zero_ifd_offset + 2;
												reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
												gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
												gTagCheckCnt = 1;	// reset seearch count

												// Get EXIF IDF offset
												ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
												if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
													// tag not find
													img_detail->ext_mp[ext_cnt].class2_width = img_detail->class2_width;
													img_detail->ext_mp[ext_cnt].class2_lines = img_detail->class2_lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
													BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EXIF IDF OFFSET SCREEN2\n"));
#endif
												}
												else if (ercd != 0) {
													// error
													break;
												}
												else {
													// get Exif IFD offset
													data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

													// get next start offset
													ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
													if (ercd != 0) {
														// error
														break;
													}

													ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELXDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
													if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
														// tag not find
														img_detail->ext_mp[ext_cnt].class2_width = img_detail->class2_width;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
														BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_X_DIMENSION SCREEN2\n"));
#endif
													}
													else if (ercd != 0) {
														// error
														break;
													}
													else {
														// get Image Width
														if (tag_type == 3) {
															// Short data
															img_detail->ext_mp[ext_cnt].class2_width = (FW_ULONG)dcf_exif_get_short_value(buffer + tag_offset + value_offset);
														}
														else {
															// Long data
															img_detail->ext_mp[ext_cnt].class2_width = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
														}
													}
													tag_offset = reset_offset;

													ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELYDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
													if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
														// tag not find
														img_detail->ext_mp[ext_cnt].class2_lines = img_detail->class2_lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
														BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_Y_DIMENSION\n"));
#endif
													}
													else if (ercd != 0) {
														// error
														break;
													}
													else {
														// get Image Height
														if (tag_type == 3) {
															// Short data
															img_detail->ext_mp[ext_cnt].class2_lines = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
														}
														else {
															// Long data
															img_detail->ext_mp[ext_cnt].class2_lines = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
														}
													}
												}

												if(first_ifd_offset != 0) {
													// get NEXT IFD Pointer
													ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + first_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
													if (ercd != 0) {
														// error
														break;
													}

													// search JPEG InterchangeFormat IFD (0x201)
													ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
													if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
														// tag not find
														ercd = 0;
													}
													else if (ercd != 0) {
														// error
														break;
													}
													else {
														// get thumbnail offset
														load_image_data->ext_mp[ext_cnt].class2_thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset) + load_image_data->ext_mp[ext_cnt].class2_offset;
													}

													// JPEG InterchangeFormatLength tag is 0x202
													ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
													if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
														// tag not find
														ercd = 0;
													}
													else if (ercd != 0) {
														// error
														break;
													}
													else {
														// get thumbnail size
														load_image_data->ext_mp[ext_cnt].class2_thumb_size = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
													}
												}
											}
											else {
												// APP1 not exist
												img_detail->ext_mp[ext_cnt].class2_width = img_detail->class2_width;
												img_detail->ext_mp[ext_cnt].class2_lines = img_detail->class2_lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
												BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND APP1 in SCREEN2\n"));
#endif
											}

											// YCBCR Format
											ercd = dcf_exif_get_ycbcr_format(file_id, buffer, load_image_data->ext_mp[ext_cnt].class2_offset, &img_detail->ext_mp[ext_cnt].class2_ycc_format);
											if (ercd != 0) {
												// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
												BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND SOF0 in YCBCR FORMAT(class2)\n"));
#endif
											}

										}
										else {
											// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
											BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
											break;
										}
									}
									else {
										// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
										BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
										break;
									}
								}
								else {
									// class2 not exist
									img_detail->ext_mp[ext_cnt].class2_width = 0;
									img_detail->ext_mp[ext_cnt].class2_lines = 0;
								}								
							}
						}						
						else {
							// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
							BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
							break;
						}				
					}
					else {
						// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
						BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
						break;
					}
				}
			}
		}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
		break;
	}

	if (ercd == FSDCF_ERRNO_NOT_APP1) {
		// Not Exif
		buffer += (app_zero_size + 2);

		// get SOF offset of buffer
		dcf_exif_get_sof_offset(buffer, &soh_offset);

		// Horizontal lines(7Byte from SOF top)
		img_detail->main_width = dcf_exif_get_short_value_big_endian(buffer + soh_offset + 0x7);
		// Vertical lines(5Byte from SOF top)
		img_detail->main_lines = dcf_exif_get_short_value_big_endian(buffer + soh_offset + 0x5);

		// ycbcr format
		img_detail->ycc_format = *(buffer + soh_offset + 0xB);

		buffer -= (app_zero_size + 2);
	}
	
	ercd2 = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)buffer);
	if (ercd2 != D_BF_FS_USER_CUSTOM_OK) {
		// memory release error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd2));
#endif
		ercd = ercd2;
	}

	return ercd;
}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

/**
 * @brief analyze exif tag offset in JPEG.
 * @param i:file_id file id
 * @param o:*load_image_data image data info
 * @param o:*img_detail image detail information
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Exif_Analyze_Jpeg(FW_INT32 file_id, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data, T_BF_DCF_IF_IMAGE_DETAIL* img_detail)
{
	FW_INT32  ercd, ercd2;
	FW_UCHAR  *buffer;
	FW_ULONG  result;
	FW_USHORT app1_size = 0;
	FW_USHORT app2_size = 0;
	FW_ULONG  zero_ifd_num = 0;
	FW_ULONG  next_ifd_ptr = 0;
	FW_ULONG  first_ifd_offset = 0;
	FW_ULONG  tag_offset = 0;
	FW_ULONG  reset_offset = 0;
	FW_ULONG  value_offset = 0;
	FW_ULONG  total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
	FW_UCHAR  tag_type;
	FW_UCHAR  tag_type_byte;
	FW_ULONG  tag_count;
	FW_ULONG  data_offset;
	FW_USHORT tmp_value;
	FW_USHORT app_zero_size = 0;
	FW_USHORT comment_size = 0;
	FW_USHORT jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
	FW_USHORT zero_ifd_offset   = D_BF_DCF_EXIF_TIFFHEADSIZE;

	FW_ULONG  soh_offset = 0;
	FW_UCHAR  app2_flag  = 0;
	FW_ULONG  total_read = 0;
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	FW_ULONG  tag_offset_save;
	FW_ULONG  value_offset_save;
	FW_ULONG  reset_offset_save;
#endif
#ifdef CO_BF_DCF_IF_MPF_ENABLE
	FW_ULONG  mp_tag_offset = 0;
	FW_ULONG  mp_head_offset;
	FW_USHORT mp_entry_num;
	FW_USHORT mp_entry_cnt;
	T_BF_DCF_EXIF_MP_ENTRY mp_entry;
#endif	// CO_BF_DCF_IF_MPF_ENABLE
	FW_CHAR tag_val_type;
#ifdef CO_BF_DCF_IF_GET_GPSTAG
	FW_ULONG  gps_ifd_offset = 0;
#endif	// CO_BF_DCF_IF_GET_GPSTAG

	if (load_image_data == NULL) {
		// parameter error
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (img_detail == NULL) {
		// parameter error
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	// data clear
#ifdef CO_BF_DCF_IF_MPF_ENABLE
	img_detail->baseline_mp_flag  = 0;
#endif	// CO_BF_DCF_IF_MPF_ENABLE

#ifdef CO_BF_DCF_IF_APP11_ENABLE
	img_detail->app11_flg = E_BF_DCF_IF_APP11_FLG_OFF;
#endif // CO_BF_DCF_IF_APP11_ENABLE

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
#endif

#ifdef CO_BF_DCF_IF_MPF_ENABLE
	load_image_data->main_size           = 0;
	load_image_data->class1_size         = 0;
	load_image_data->class1_offset       = 0;
	load_image_data->class1_thumb_size   = 0;
	load_image_data->class1_thumb_offset = 0;
	load_image_data->class2_size         = 0;
	load_image_data->class2_offset       = 0;
	load_image_data->class2_thumb_size   = 0;
	load_image_data->class2_thumb_offset = 0;
#endif	// CO_BF_DCF_IF_MPF_ENABLE

	memset(&img_detail->exiftag_flg, 0x00, sizeof(FW_ULONG));
	memset(&img_detail->firstIFD_flg, 0x00, sizeof(FW_ULONG));
	
	// memory allocation
	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&buffer);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		// memory get error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	while (1) {
		// read data from Exif
		// file pointer : file top
		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
			// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		BF_L1l2cache_Flush_Addr((UINT32)buffer, (UINT32)D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);

		// check DQT marker
		if (*(buffer + 3) == 0xDB) {
			// detect JPEG(without APPx) format
			img_detail->jpeg_format = E_BF_DCF_IF_JPEG_FORMAT_NOT_EXIF;
			dcf_exif_exif_info_dummy(img_detail);
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;

		}
		// check APP0 marker
		else if (*(buffer + 3) == 0xE0) {
			// detect APP0
			app_zero_size = dcf_exif_get_short_value_big_endian(buffer + 4) + 2;
			jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET + app_zero_size;

			// check COM marker
			if (*(buffer + app_zero_size + 3) == 0xFE) {
				comment_size = dcf_exif_get_short_value_big_endian(buffer + app_zero_size + 4) + 2;
				app_zero_size = app_zero_size + comment_size;
			}
		}
		else {
			; // DO NOTHING
		}
		
		// check DQT marker
		if (*(buffer + app_zero_size + 3) == 0xDB) {
			// detect JFIF(no thumbnail) format
			img_detail->jpeg_format = E_BF_DCF_IF_JPEG_FORMAT_NOT_EXIF;
			dcf_exif_exif_info_dummy(img_detail);
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;

		}
		// check APP0 marker
		else if (*(buffer + app_zero_size + 3) == 0xE0) {
			// detect JFIF(JFXX) format
			img_detail->jpeg_format = E_BF_DCF_IF_JPEG_FORMAT_NOT_EXIF;
			dcf_exif_exif_info_dummy(img_detail);
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;

		}
		// check APP1 marker
		// APP1 marker's place is next to SOI marker
		else if (*(buffer + app_zero_size + 3) != 0xE1) {
			// not detect APP1
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND APP1 MARKER\n"));
#endif
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;
		}
		else {
			; // DO NOTHING
		}

		app1_size = dcf_exif_get_short_value_big_endian((buffer + app_zero_size + 4));

		// reset pointer
		ercd = BF_Mdf_Wrapper_Lseek(file_id, (4 + app_zero_size + app1_size), SEEK_SET, &result);
		if (ercd != 0) {
			// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
			// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}
		BF_L1l2cache_Flush_Addr((UINT32)buffer, (UINT32)D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);

		if (*(buffer + 1) == 0xE2) {
			// detect APP2
			app2_size = dcf_exif_get_short_value_big_endian(buffer + 2);
			app2_flag = 1;
			//total_read += (2 + app2_size);

#ifdef CO_BF_DCF_IF_MPF_ENABLE
			if (strcmp((const FW_CHAR*)(buffer + 4), "MPF") == 0) {
				// MPF format
				img_detail->baseline_mp_flag = 1;
				mp_head_offset = (4 + app1_size + 8);

				g_bf_dcf_exif_big_or_lit = *(FW_USHORT*)(buffer + 8);
				mp_tag_offset = dcf_exif_get_long_value(buffer + 12) + 10;
				gNumOfTag = dcf_exif_get_short_value(buffer + mp_tag_offset - 2);	// Num of MP index IFD's tag
				gTagCheckCnt = 1;	// reset seearch count

				// Check MP Entry Num
				ercd = dcf_exif_check_exif_tag(file_id, buffer, 0xB001, &mp_tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
				if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
					// MP Entry num not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND MP ENTRY NUM\n"));
#endif
					break;
				}
				else if (ercd != 0) {
					// error
					break;
				}
				else {
					// get MP entry num
					mp_entry_num = dcf_exif_get_long_value(buffer + mp_tag_offset + value_offset);
				}
				tag_offset = reset_offset;

				// Check MP Entry
				ercd = dcf_exif_check_exif_tag(file_id, buffer, 0xB002, &mp_tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
				if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
					// MP entry not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND MP ENTRY\n"));
#endif
					break;
				}
				else if (ercd != 0) {
					// error
					break;
				}
				else {
					// get MP entry
					data_offset = 8 + dcf_exif_get_long_value(buffer + mp_tag_offset + value_offset);

					for (mp_entry_cnt = 0; mp_entry_cnt < mp_entry_num; mp_entry_cnt++) {
						if (mp_entry_cnt >= 3) {
							// MP entry num over 3 (not Baseline MP format)
							break;
						}
						mp_entry.mp_info        = dcf_exif_get_long_value(buffer + data_offset + (16 * mp_entry_cnt));
						mp_entry.mp_data_size   = dcf_exif_get_long_value(buffer + data_offset + (16 * mp_entry_cnt) + 4);
						mp_entry.mp_data_offset = dcf_exif_get_long_value(buffer + data_offset + (16 * mp_entry_cnt) + 8);
						mp_entry.mp_entry_1     = dcf_exif_get_short_value(buffer + data_offset + (16 * mp_entry_cnt) + 12);
						mp_entry.mp_entry_2     = dcf_exif_get_short_value(buffer + data_offset + (16 * mp_entry_cnt) + 14);

						if ((mp_entry.mp_info & 0x000FFFFF) == 0x00030000) {
							// Baseline Main Image
							load_image_data->main_size = mp_entry.mp_data_size;
						}
						else if ((mp_entry.mp_info & 0x000FFFFF) == 0x00010001) {
							// ScreenNail (VGA)
							load_image_data->class1_size   = mp_entry.mp_data_size;
							load_image_data->class1_offset = mp_entry.mp_data_offset + mp_head_offset;
//							img_detail->baseline_mp_flag |= 0x02;
						}
						else if ((mp_entry.mp_info & 0x000FFFFF) == 0x00010002) {
							// ScreenNail (Full HD)
							load_image_data->class2_size   = mp_entry.mp_data_size;
							load_image_data->class2_offset = mp_entry.mp_data_offset + mp_head_offset;
//							img_detail->baseline_mp_flag |= 0x04;
						}
						else {
							; // DO NOTHING
						}
					}
				}
				g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
			}
			else {
				; // DO NOTHING
			}
#endif	// CO_BF_DCF_IF_MPF_ENABLE
		}

		if (app2_flag == 1) {
			// reset pointer
			ercd = BF_Mdf_Wrapper_Lseek(file_id, (4 + app_zero_size + app1_size + 2 + app2_size), SEEK_SET, &result);
			if (ercd != 0) {
				// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				break;
			}

			ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
			if (ercd != 0) {
				// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				break;
			}
			BF_L1l2cache_Flush_Addr((UINT32)buffer, (UINT32)D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}

#ifdef CO_BF_DCF_IF_APP11_ENABLE
		if ( (*(buffer) == 0xFF) && (*(buffer + 1) == 0xEB) ) {
			// Exist APP11 Marker
			img_detail->app11_flg = E_BF_DCF_IF_APP11_FLG_ON;
		}
#endif // CO_BF_DCF_IF_APP11_ENABLE

		while ((*(buffer + 1) != 0xDB) && (*(buffer + 1) != 0xC0) && (*(buffer + 1) != 0xC4)) {
			if (*(buffer) != 0xFF) {
				// not find SOF marker
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND SOF MARKER\n"));
#endif
				ercd = FSDCF_ERRNO_NOT_APP1;
				break;
			}
			total_read += dcf_exif_get_short_value_big_endian(buffer + 2) + 2;

			if (app2_flag == 1) {
				// APP2 exist
				// reset pointer
				ercd = BF_Mdf_Wrapper_Lseek(file_id, (4 + app_zero_size + app1_size + 2 + app2_size + total_read), SEEK_SET, &result);
				if (ercd != 0) {
					// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
					break;
				}
			}
			else {
				// APP2 not exist
				// reset pointer
				ercd = BF_Mdf_Wrapper_Lseek(file_id, (4 + app_zero_size + app1_size + total_read), SEEK_SET, &result);
				if (ercd != 0) {
					// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
					break;
				}
			}

			ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
			if (ercd != 0) {
				// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				break;
			}
			BF_L1l2cache_Flush_Addr((UINT32)buffer, (UINT32)D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}

		//soh_offset = total_read;
		// get SOF offset of buffer
		dcf_exif_get_sof_offset(buffer, &soh_offset);

		// Horizontal lines(7Byte from SOF top)
		img_detail->main_width = dcf_exif_get_short_value_big_endian(buffer + soh_offset + 0x7);
		// Vertical lines(5Byte from SOF top)
		img_detail->main_lines = dcf_exif_get_short_value_big_endian(buffer + soh_offset + 0x5);

		// ycbcr format
		img_detail->ycc_format = *(buffer + soh_offset + 0xB);

		// detect EXIF format
		img_detail->jpeg_format = E_BF_DCF_IF_JPEG_FORMAT_EXIF;

		// reset pointer
		ercd = BF_Mdf_Wrapper_Lseek(file_id, 0, SEEK_SET, &result);
		if (ercd != 0) {
			// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
			// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}
		BF_L1l2cache_Flush_Addr((UINT32)buffer, (UINT32)D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);

		// check endian
		g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));
		img_detail->byte_order = g_bf_dcf_exif_big_or_lit;

		zero_ifd_offset  = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);
		zero_ifd_num     = dcf_exif_get_short_value(buffer + jpeg_start_offset + zero_ifd_offset);
		next_ifd_ptr     = jpeg_start_offset + zero_ifd_offset + 2 + zero_ifd_num * sizeof(T_BF_DCF_EXIF_IFD);	/* point Next IFD Pointer */
		first_ifd_offset = dcf_exif_get_long_value(buffer + next_ifd_ptr);
		//check whether the image has 1st IFD
		if ( first_ifd_offset != 0 ){
		 	img_detail->firstIFD_flg = 1;
		}

		tag_offset   = jpeg_start_offset + zero_ifd_offset + 2;
		reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
		gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
		gTagCheckCnt = 1;	// reset seearch count

		// Get Maker
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_MAKE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND MAKE \n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Maker
			img_detail->exiftag_flg |= FJ_EXIF_TAG_MAKE_FLAG;
			dcf_exif_check_tag_value_type(tag_type, tag_count, &tag_val_type);
			if(tag_val_type == D_BF_DCF_EXIF_TYPE_VALUE){
				dcf_exif_tag_type_convert_byte(tag_type, &tag_type_byte);
				memcpy(&img_detail->maker, (void*)(buffer + tag_offset + value_offset), (tag_type_byte * tag_count));
			}
			else{
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				memcpy(&img_detail->maker, (void*)(buffer + data_offset), D_BF_DCF_EXIF_V_MAKE_S - 1);
			}
		}
		tag_offset = reset_offset;

		// Get Model
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_MODEL_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND MODEL \n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Model
			img_detail->exiftag_flg |= FJ_EXIF_TAG_MODEL_FLAG;
			dcf_exif_check_tag_value_type(tag_type, tag_count, &tag_val_type);
			if(tag_val_type == D_BF_DCF_EXIF_TYPE_VALUE){
				dcf_exif_tag_type_convert_byte(tag_type, &tag_type_byte);
				memcpy(&img_detail->model, (void*)(buffer + tag_offset + value_offset), (tag_type_byte * tag_count));
			}
			else{
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				memcpy(&img_detail->model, (void*)(buffer + data_offset), D_BF_DCF_EXIF_V_MODEL_S - 1);
			}
		}
		tag_offset = reset_offset;

		// Get Orientation
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_ORIENT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND ORIENTATION\n"));
#endif
			//img_detail->rotate = E_BF_DCF_IF_ROTATE_0;
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Orientation
			img_detail->exiftag_flg |= FJ_EXIF_TAG_ORIENT_FLAG;
			tmp_value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			//if(tmp_value < 9) {
			//	img_detail->rotate = (E_BF_DCF_IF_ROTATE)tmp_value;
			//}
			//else {
			//	img_detail->rotate = E_BF_DCF_IF_ROTATE_0;
			//}
			img_detail->rotate = (E_BF_DCF_IF_ROTATE)tmp_value;
		}
		tag_offset = reset_offset;

		// Get date & time
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_DATETIME_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EDIT DATE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get date & time
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_date_value(file_id, buffer, &data_offset, total_buffer, &img_detail->edit_date_time);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_DATETIME_FLAG;
		}
		tag_offset = reset_offset;

		// Get EXIF IDF offset
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND EXIF IDF OFFSET\n"));
#endif
			break;
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// set exif tag flag
			img_detail->exiftag_flg |= FJ_EXIF_TAG_EXIFPTR_FLAG;
		}
#ifdef CO_BF_DCF_IF_GET_GPSTAG
		tag_offset_save   = tag_offset;
		value_offset_save = value_offset;
		reset_offset_save = reset_offset;

		tag_offset = reset_offset;

		// Get GPS IDF offset
		ercd = dcf_exif_check_exif_tag(file_id, buffer, 0x8825, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("BF_Dcf_Exif_Analyze_Jpeg() CAN NOT FIND GPS IFD OFFSET  %d\n",img_detail->file_info.file_no ));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			gps_ifd_offset = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
		}

		tag_offset   = tag_offset_save;
		value_offset = value_offset_save;
		reset_offset = reset_offset_save;
#endif	// CO_BF_DCF_IF_GET_GPSTAG

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		/* data save */
		tag_offset_save   = tag_offset;
		value_offset_save = value_offset;
		reset_offset_save = reset_offset;

		tag_offset = reset_offset;

		// Get PrintIM
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_PRINTIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PRINTIM\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get PrintIM
			img_detail->exiftag_flg |= FJ_EXIF_TAG_PRINTIM_FLAG;
			dcf_exif_check_tag_value_type(tag_type, tag_count, &tag_val_type);
			if(tag_val_type == D_BF_DCF_EXIF_TYPE_VALUE){
				dcf_exif_tag_type_convert_byte(tag_type, &tag_type_byte);
				memcpy(&img_detail->PrintIM, (void*)(buffer + tag_offset + value_offset), (tag_type_byte * tag_count));
			}
			else{
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				memcpy( &img_detail->PrintIM, (void*)(buffer + data_offset), D_BF_DCF_EXIF_V_PRINTIM_S - 1);
			}
		}
		// data restore
		tag_offset   = tag_offset_save;
		value_offset = value_offset_save;
		reset_offset = reset_offset_save;
#endif

		data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

		// get next start offset
		ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
		if (ercd != 0) {
			// error
			break;
		}

		// ExposureTime
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_EXPTIME_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EXPOSURE TIME\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Exposure Time
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, &img_detail->EpTimeNum, &img_detail->EpTimeDen);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_EXPTIME_FLAG;
		}
		tag_offset = reset_offset;

		// F Value
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_FNUM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND F_VALUE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get F Value
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, &img_detail->FvNum, &img_detail->FvDen);
			if (ercd != 0) {
				// gat value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_FNUM_FLAG;
		}
		tag_offset = reset_offset;

		// ExposureProgram
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_EXPPROG_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EXPOSURE_PROGRAM\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Exposure Program
			img_detail->exiftag_flg |= FJ_EXIF_TAG_EXPPROG_FLAG;
			img_detail->EpProgram = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
		tag_offset = reset_offset;

		// ISO speed ratings
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_ISOSPEEDRATE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND ISO SPEED RATINGS\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get ISO speed ratings
			img_detail->exiftag_flg |= FJ_EXIF_TAG_ISOSPEEDRATE_FLAG;
			//img_detail->iso = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			dcf_exif_tag_type_convert_byte(tag_type, &tag_type_byte);
			if ((tag_type_byte * tag_count) > 4) {
				// data size over 4byte
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				img_detail->iso = dcf_exif_get_short_value(buffer + data_offset);
			}
			else {
				// data size under 4byte
				img_detail->iso = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			}
		}
		tag_offset = reset_offset;

		// Get date & time Original
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_DATETIMEORIG_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND DATE TIME ORIGINAL\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get date & time Original
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_date_value(file_id, buffer, &data_offset, total_buffer, &img_detail->DateTimeOriginal);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_DATETIMEORIG_FLAG;
		}
		tag_offset = reset_offset;

		// Get date & time Digital
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_DATETIMEDITI_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND DATE TIME DIGITAL\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get date & time Digital
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_date_value(file_id, buffer, &data_offset, total_buffer, &img_detail->DateTimeDigital);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_DATETIMEDITI_FLAG;
		}
		tag_offset = reset_offset;

		// CompressedBitParPixel
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_COMPRESSBPP_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND BIT PAR PIXEL\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Compressed Bit Per Pixel
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, &img_detail->bitperpixelNum, &img_detail->bitperpixelDen);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_COMPRESSBPP_FLAG;
		}
		tag_offset = reset_offset;

		// ShutterSpeedValue
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_SHUTTER_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND SHUTTER SPEED\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get ShutterSpeedValue
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, &img_detail->ShutterSpeedNum, &img_detail->ShutterSpeedDen);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_SHUTTERSPEED_FLAG;
		}
		tag_offset = reset_offset;

		// Exposure bias
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_EXPBIAS_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND F_VALUE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Exposure bias
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
#if 0
			ercd = dcf_exif_get_exif_set_value_ex(file_id, buffer, &data_offset, total_buffer, &img_detail->ExposureBiasNum, &img_detail->ExposureBiasDen);
#endif
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->ExposureBiasNum, (FW_ULONG*)&img_detail->ExposureBiasDen);
			if (ercd != 0) {
				// get value error
				break;
			}
			img_detail->exiftag_flg |= FJ_EXIF_TAG_EXPBIAS_FLAG;
		}
		tag_offset = reset_offset;

		// MeteringMode
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_METERMODE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND METERING_MODE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Metering Mode
			img_detail->exiftag_flg |= FJ_EXIF_TAG_METERMODE_FLAG;
			img_detail->MeteringMode = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
		tag_offset = reset_offset;

		// LightSource
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_LIGHTSOURCE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND LIGHT_SOURCE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Light Source
			img_detail->exiftag_flg |= FJ_EXIF_TAG_LIGHTSOURCE_FLAG;
			img_detail->LightSource = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
		tag_offset = reset_offset;

		// Flash
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_FLASH_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND FLASH\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Flash
			img_detail->exiftag_flg |= FJ_EXIF_TAG_FLASH_FLAG;
			img_detail->flash = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
		tag_offset = reset_offset;

		// Maker Note
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_MAKERNOTE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND MAKERNOTE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Maker Note (offset & length)
			img_detail->exiftag_flg |= FJ_EXIF_TAG_MAKERNOTE_FLAG;
			img_detail->maker_note_offset = (FW_ULONG)(D_BF_DCF_EXIF_TIFFOFFSET + dcf_exif_get_long_value(buffer + tag_offset + value_offset));
			img_detail->maker_note_length = tag_count;
		}
		tag_offset = reset_offset;

		// Image Width */
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELXDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_X_DIMENSION\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Image Width
			img_detail->exiftag_flg |= FJ_EXIF_TAG_PEXELXDIM_FLAG;
			if (tag_type == 3) {
				// Short data
				img_detail->width = (FW_ULONG)dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			}
			else {
				// Long data
				img_detail->width = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			}
		}
		tag_offset = reset_offset;

		// Image Height
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELYDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_Y_DIMENSION\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Image Height
			img_detail->exiftag_flg |= FJ_EXIF_TAG_PEXELYDIM_FLAG;
			if (tag_type == 3) {
				// Short data
				img_detail->lines = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			}
			else {
				// Long data
				img_detail->lines = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			}
		}

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		// Custom Image Processing
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_CUSTOMRENDERED_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND CUSTOM RENDERED\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Custom Image Processing
			img_detail->exiftag_flg |= FJ_EXIF_TAG_CUSTOMRENDERED_FLAG;
			img_detail->CustomRendered = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
#endif

		// WhiteBalanceMode
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_WB_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND WHITE_BALANCE_MODE\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get White Balance Mode
			img_detail->exiftag_flg |= FJ_EXIF_TAG_WB_FLAG;
			img_detail->WBMode = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
		tag_offset = reset_offset;

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
		// Contrast
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_CONTRAST_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND CONTRAST\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Contrast
			img_detail->exiftag_flg |= FJ_EXIF_TAG_CONTRAST_FLAG;
			img_detail->Contrast = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}

		// Saturation
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_SATURATION_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND SATURATION\n"));
#endif
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get Saturation
			img_detail->exiftag_flg |= FJ_EXIF_TAG_SATURATION_FLAG;
			img_detail->Saturation = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
		}
#endif

#ifdef CO_BF_DCF_IF_GET_GPSTAG
		img_detail->gpstag_flg = 0;
		if (gps_ifd_offset != 0) {
			ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + gps_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
			if (ercd != 0) {
				// error
				break;
			}

			// Get GPSLatitudeRef
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_GPS_TAG_GPSLATITUDEREF_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND GPSLatitudeRef \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get GPSLatitudeRef
				img_detail->gpstag_flg |= FJ_GPS_TAG_GPSLATITUDEREF_FLAG;
				memcpy(&img_detail->gpsLatitudeRef, (void*)(buffer + tag_offset + value_offset), 2);
			}
			tag_offset = reset_offset;

			// Get GPSLatitude
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_GPS_TAG_GPSLATITUDE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND GPSLatitude \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get GPSLatitude
				T_BF_DCF_EXIF_GPS_LATITUDE_LONGITUDE coordinate_value;

				img_detail->gpstag_flg |= FJ_GPS_TAG_GPSLATITUDE_FLAG;
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
#if 1
				ercd = dcf_exif_get_gps_coordinate_value(file_id, buffer, &data_offset, total_buffer, &coordinate_value);
				img_detail->gpsLatitudeDdNum = coordinate_value.DdNum;
				img_detail->gpsLatitudeDdDen = coordinate_value.DdDen;
				img_detail->gpsLatitudeMmNum = coordinate_value.MmNum;
				img_detail->gpsLatitudeMmDen = coordinate_value.MmDen;
				img_detail->gpsLatitudeSsNum = coordinate_value.SsNum;
				img_detail->gpsLatitudeSsDen = coordinate_value.SsDen;

#else
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsLatitudeDdNum, (FW_ULONG*)&img_detail->gpsLatitudeDdDen);
				data_offset += 8;
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsLatitudeMmNum, (FW_ULONG*)&img_detail->gpsLatitudeMmDen);
				data_offset += 8;
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsLatitudeSsNum, (FW_ULONG*)&img_detail->gpsLatitudeSsDen);
#endif
			}
			tag_offset = reset_offset;

			// Get gpsLongitudeRef
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_GPS_TAG_GPSLONGITUDEREF_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND gpsLongitudeRef \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get gpsLongitudeRef
				img_detail->gpstag_flg |= FJ_GPS_TAG_GPSLONGITUDEREF_FLAG;
				memcpy(&img_detail->gpsLongitudeRef, (void*)(buffer + tag_offset + value_offset), 2);
			}
			tag_offset = reset_offset;

			// Get gpsLongitude
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_GPS_TAG_GPSLONGITUDE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND gpsLongitude \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get gpsLongitude
				T_BF_DCF_EXIF_GPS_LATITUDE_LONGITUDE coordinate_value;

				img_detail->gpstag_flg |= FJ_GPS_TAG_GPSLONGITUDE_FLAG;
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

#if 1
				ercd = dcf_exif_get_gps_coordinate_value(file_id, buffer, &data_offset, total_buffer, &coordinate_value);
				img_detail->gpsLongitudeDdNum = coordinate_value.DdNum;
				img_detail->gpsLongitudeDdDen = coordinate_value.DdDen;
				img_detail->gpsLongitudeMmNum = coordinate_value.MmNum;
				img_detail->gpsLongitudeMmDen = coordinate_value.MmDen;
				img_detail->gpsLongitudeSsNum = coordinate_value.SsNum;
				img_detail->gpsLongitudeSsDen = coordinate_value.SsDen;
#else
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsLongitudeDdNum, (FW_ULONG*)&img_detail->gpsLongitudeDdDen);
				data_offset += 8;
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsLongitudeMmNum, (FW_ULONG*)&img_detail->gpsLongitudeMmDen);
				data_offset += 8;
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsLongitudeSsNum, (FW_ULONG*)&img_detail->gpsLongitudeSsDen);
#endif
			}
			tag_offset = reset_offset;

			// Get gpsImgDirectionRef
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_GPS_TAG_GPSIMGDIRECTIONREF_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND gpsImgDirectionRef \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get gpsImgDirectionRef
				img_detail->gpstag_flg |= FJ_GPS_TAG_GPSIMGDIRECTIONREF_FLAG;
				memcpy(&img_detail->gpsImgDirectionRef, (void*)(buffer + tag_offset + value_offset), 2);
			}
			tag_offset = reset_offset;

			// Get gpsImgDirection
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_GPS_TAG_GPSIMGDIRECTION_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND gpsImgDirection \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get gpsImgDirection
				img_detail->gpstag_flg |= FJ_GPS_TAG_GPSIMGDIRECTION_FLAG;
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer, (FW_ULONG*)&img_detail->gpsImgDirectionNum, (FW_ULONG*)&img_detail->gpsImgDirectionDen);
			}
			tag_offset = reset_offset;
		}
#endif	// CO_BF_DCF_IF_GET_GPSTAG

		// get NEXT IFD Pointer
		ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + first_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
		if (ercd != 0) {
			// error
			break;
		}

		if( (tag_offset - 0xE) != 0) {
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
			// Get Maker1
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_MAKER1_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
				// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND MAKER1 \n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get Maker (IFD1)
				img_detail->exiftag_flg |= FJ_EXIF_TAG_MAKER1_FLAG;
				dcf_exif_check_tag_value_type(tag_type, tag_count, &tag_val_type);
				if(tag_val_type == D_BF_DCF_EXIF_TYPE_VALUE){
					dcf_exif_tag_type_convert_byte(tag_type, &tag_type_byte);
					memcpy(&img_detail->maker1, (void*)(buffer + tag_offset + value_offset), (tag_type_byte * tag_count));
				}
				else{
					data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
					memcpy(&img_detail->maker1, (void*)(buffer + data_offset), D_BF_DCF_EXIF_V_MAKE_S - 1);
				}
			}
			tag_offset = reset_offset;

			// Get Model1
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_MODEL1_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
				// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND MODEL1\n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get Model (IFD1)
				img_detail->exiftag_flg |= FJ_EXIF_TAG_MODEL1_FLAG;
				dcf_exif_check_tag_value_type(tag_type, tag_count, &tag_val_type);
				if(tag_val_type == D_BF_DCF_EXIF_TYPE_VALUE){
					dcf_exif_tag_type_convert_byte(tag_type, &tag_type_byte);
					memcpy(&img_detail->model1, (void*)(buffer + tag_offset + value_offset), (tag_type_byte * tag_count));
				}
				else{
					data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
					memcpy(&img_detail->model1, (void*)(buffer + data_offset), D_BF_DCF_EXIF_V_MODEL_S - 1);
				}
			}
			tag_offset = reset_offset;
#endif

			// Get Orientation
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_ORIENT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
				// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND ORIENTATION\n"));
#endif
//			img_detail->rotate_thumb = E_BF_DCF_IF_ROTATE_0;
//			img_detail->rotate_thumb = img_detail->rotate;
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get Orientation (IFD1)
				img_detail->exiftag_flg |= FJ_EXIF_TAG_ORIENTTHM_FLAG;
				tmp_value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
				//if(tmp_value < 9) {
				//	img_detail->rotate_thumb = (E_BF_DCF_IF_ROTATE)tmp_value;
				//}
				//else {
				//	img_detail->rotate_thumb = E_BF_DCF_IF_ROTATE_0;
				//}
				img_detail->rotate_thumb = (E_BF_DCF_IF_ROTATE)tmp_value;
			}
			tag_offset = reset_offset;

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
			// Get date & time 1
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_DATETIME1_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
				// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
				BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EDIT DATE1\n"));
#endif
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get date & time (IFD1)
				data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				ercd = dcf_exif_get_exif_date_value(file_id, buffer, &data_offset, total_buffer, &img_detail->date_time1);
				if (ercd != 0) {
					break;
				}
				img_detail->exiftag_flg |= FJ_EXIF_TAG_DATETIME1_FLAG;
			}
			tag_offset = reset_offset;
#endif

			// search JPEG InterchangeFormat IFD (0x201)
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
				// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND JPEG INTER_CHANGE_FORMAT\n"));
#endif
				break;
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get JPEG Interchange Format
				img_detail->exiftag_flg |= FJ_EXIF_TAG_JPG_INTERCHGFMT_FLAG;
			}
			load_image_data->thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

			// JPEG InterchangeFormatLength tag is 0x202
			ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
			if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
				// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND JPEG INTER_CHANGE_FORMAT_LENGTH\n"));
#endif
				break;
			}
			else if (ercd != 0) {
				// error
				break;
			}
			else {
				// get JPEG Interchange Format Length
				img_detail->exiftag_flg |= FJ_EXIF_TAG_JPG_INTERCHGFMTLEN_FLAG;
			}

			load_image_data->thumb_size = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = BF_Mdf_Wrapper_Lseek(file_id, load_image_data->thumb_offset, SEEK_SET, &result);
			if (ercd == 0) {
				FW_ULONG read_size = (load_image_data->thumb_size < D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE) ?
									 load_image_data->thumb_size : D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
				ercd = BF_Mdf_Wrapper_Read(file_id, buffer, read_size, &result);
				if (ercd == 0) {
					BF_L1l2cache_Flush_Addr((UINT32)buffer, (UINT32)read_size);
					img_detail->ycc_format_thum = dcf_exif_get_thum_ycc_format(buffer, read_size);
				}
				else {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				}
			}
			else {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			}
		}
		else {
			// Not 1st IFD
			load_image_data->thumb_size = 0;
			load_image_data->thumb_offset = 0;
		}

#ifdef CO_BF_DCF_IF_MPF_ENABLE
		// YCBCR Format
		img_detail->class1_ycc_format = 0;
		img_detail->class2_ycc_format = 0;

		if (load_image_data->class1_size != 0) {
			// VGA ScreenNail Exist
			ercd = BF_Mdf_Wrapper_Lseek(file_id, load_image_data->class1_offset, SEEK_SET, &result);
			if (ercd == 0) {
				// lseek OK
				ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
				if (ercd == 0) {
					BF_L1l2cache_Flush_Addr((UINT32)buffer, (UINT32)D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);

					// read OK
					if ((*(buffer + 2) == 0xFF) && (*(buffer + 3) == 0xE1)) {
						// APP1 Exist
						jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
						g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));
						total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;

						zero_ifd_offset  = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);
						zero_ifd_num     = dcf_exif_get_short_value(buffer + jpeg_start_offset + zero_ifd_offset);
						next_ifd_ptr     = jpeg_start_offset + zero_ifd_offset + 2 + zero_ifd_num * sizeof(T_BF_DCF_EXIF_IFD);	/* point Next IFD Pointer */
						first_ifd_offset = dcf_exif_get_long_value(buffer + next_ifd_ptr);

						tag_offset   = jpeg_start_offset + zero_ifd_offset + 2;
						reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
						gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
						gTagCheckCnt = 1;	// reset seearch count

						/* Get EXIF IDF offset */
						ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
						if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
							// tag not find
							img_detail->class1_width = img_detail->width;
							img_detail->class1_lines = img_detail->lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
							BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EXIF IDF OFFSET SCREEN1\n"));
#endif
						}
						else if (ercd != 0) {
							// error
							break;
						}
						else {
							// get Exif IFD offset
							data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

							// get next start offset
							ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
							if (ercd != 0) {
								// error
								break;
							}

							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELXDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								img_detail->class1_width = img_detail->width;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
								BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_X_DIMENSION\n"));
#endif
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get Image Width
								if (tag_type == 3) {
									// Short data
									img_detail->class1_width = (FW_ULONG)dcf_exif_get_short_value(buffer + tag_offset + value_offset);
								}
								else {
									// Long data
									img_detail->class1_width = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
								}
							}
							tag_offset = reset_offset;

							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELYDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								img_detail->class1_lines = img_detail->lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
								BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_Y_DIMENSION\n"));
#endif
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get Image Height
								if (tag_type == 3) {
									// Short data
									img_detail->class1_lines = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
								}
								else {
									// Long data
									img_detail->class1_lines = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
								}
							}
						}

						if(first_ifd_offset != 0) {
							// get NEXT IFD Pointer
							ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + first_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
							if (ercd != 0) {
								// error
								break;
							}

							// search JPEG InterchangeFormat IFD (0x201)
							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								ercd = 0;
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get thumbnail offset
								load_image_data->class1_thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset) + load_image_data->class1_offset;
							}

							// JPEG InterchangeFormatLength tag is 0x202
							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								ercd = 0;
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get thumbnail size
								load_image_data->class1_thumb_size = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
							}
						}
					}
					else {
						// APP1 not exist
						img_detail->class1_width = img_detail->width;
						img_detail->class1_lines = img_detail->lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
						BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND APP1 in SCREEN1\n"));
#endif
					}

					// YCBCR Format
					ercd = dcf_exif_get_ycbcr_format(file_id, buffer, load_image_data->class1_offset, &img_detail->class1_ycc_format);
					if (ercd != 0) {
						// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
						BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND SOF0 in YCBCR FORMAT(class1)\n"));
#endif
					}

				}
				else {
					// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
					break;
				}
			}
			else {
				// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				break;
			}
		}
		else {
			// class1 not exist
			img_detail->class1_width = 0;
			img_detail->class1_lines = 0;
		}
		if (load_image_data->class2_size != 0) {
			// FullHD ScreenNail Exist
			ercd = BF_Mdf_Wrapper_Lseek(file_id, load_image_data->class2_offset, SEEK_SET, &result);
			if (ercd == 0) {
				// lseek OK
				ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
				if (ercd == 0) {
					BF_L1l2cache_Flush_Addr((UINT32)buffer, (UINT32)D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);

					// read OK
					if ((*(buffer + 2) == 0xFF) && (*(buffer + 3) == 0xE1)) {
						// APP1 Exist
						jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
						g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));
						total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;

						zero_ifd_offset  = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);
						zero_ifd_num     = dcf_exif_get_short_value(buffer + jpeg_start_offset + zero_ifd_offset);
						next_ifd_ptr     = jpeg_start_offset + zero_ifd_offset + 2 + zero_ifd_num * sizeof(T_BF_DCF_EXIF_IFD);	// point Next IFD Pointer
						first_ifd_offset = dcf_exif_get_long_value(buffer + next_ifd_ptr);

						tag_offset   = jpeg_start_offset + zero_ifd_offset + 2;
						reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
						gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
						gTagCheckCnt = 1;	// reset seearch count

						// Get EXIF IDF offset
						ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
						if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
							// tag not find
							img_detail->class2_width = img_detail->width;
							img_detail->class2_lines = img_detail->lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
							BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EXIF IDF OFFSET SCREEN2\n"));
#endif
						}
						else if (ercd != 0) {
							// error
							break;
						}
						else {
							// get Exif IFD offset
							data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

							// get next start offset
							ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
							if (ercd != 0) {
								// error
								break;
							}

							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELXDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								img_detail->class2_width = img_detail->width;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
								BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_X_DIMENSION SCREEN2\n"));
#endif
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get Image Width
								if (tag_type == 3) {
									// Short data
									img_detail->class2_width = (FW_ULONG)dcf_exif_get_short_value(buffer + tag_offset + value_offset);
								}
								else {
									// Long data
									img_detail->class2_width = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
								}
							}
							tag_offset = reset_offset;

							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELYDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								img_detail->class2_lines = img_detail->lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
								BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_Y_DIMENSION\n"));
#endif
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get Image Height
								if (tag_type == 3) {
									// Short data
									img_detail->class2_lines = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
								}
								else {
									// Long data
									img_detail->class2_lines = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
								}
							}
						}

						if(first_ifd_offset != 0) {
							// get NEXT IFD Pointer
							ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + first_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
							if (ercd != 0) {
								// error
								break;
							}

							// search JPEG InterchangeFormat IFD (0x201)
							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								ercd = 0;
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get thumbnail offset
								load_image_data->class2_thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset) + load_image_data->class2_offset;
							}

							// JPEG InterchangeFormatLength tag is 0x202
							ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
							if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
								// tag not find
								ercd = 0;
							}
							else if (ercd != 0) {
								// error
								break;
							}
							else {
								// get thumbnail size
								load_image_data->class2_thumb_size = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
							}
						}
					}
					else {
						// APP1 not exist
						img_detail->class2_width = img_detail->width;
						img_detail->class2_lines = img_detail->lines;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
						BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND APP1 in SCREEN2\n"));
#endif
					}

					// YCBCR Format
					ercd = dcf_exif_get_ycbcr_format(file_id, buffer, load_image_data->class2_offset, &img_detail->class2_ycc_format);
					if (ercd != 0) {
						// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
						BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND SOF0 in YCBCR FORMAT(class2)\n"));
#endif
					}

				}
				else {
					// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
					break;
				}
			}
			else {
				// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
				BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
				break;
			}
		}
		else {
			// class2 not exist
			img_detail->class2_width = 0;
			img_detail->class2_lines = 0;
		}
#endif	// CO_BF_DCF_IF_MPF_ENABLE
		break;
	}

	if (ercd == FSDCF_ERRNO_NOT_APP1) {
		// Not Exif
		buffer += (app_zero_size + 2);

		// get SOF offset of buffer
		dcf_exif_get_sof_offset(buffer, &soh_offset);

		// Horizontal lines(7Byte from SOF top)
		img_detail->main_width = dcf_exif_get_short_value_big_endian(buffer + soh_offset + 0x7);
		// Vertical lines(5Byte from SOF top)
		img_detail->main_lines = dcf_exif_get_short_value_big_endian(buffer + soh_offset + 0x5);

		// ycbcr format
		img_detail->ycc_format = *(buffer + soh_offset + 0xB);

		buffer -= (app_zero_size + 2);
	}

	ercd2 = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)buffer);
	if (ercd2 != D_BF_FS_USER_CUSTOM_OK) {
		// memory release error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd2));
#endif
		ercd = ercd2;
	}

	return ercd;
}

/**
 * @brief set exif of jpeg.
 * @param i:thumb_size thumbnail data size
 * @param i:main_size main data size
 * @param i:vga_size vga data size
 * @param i:exif_info setting exif information
 * @param o:*jpg_head the pointer of jpeg header
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Exif_Make_Jpeg(T_BF_DCF_IF_JPEG_DATA* jpg_data)
{
	FW_INT32 ercd = 0;
	FW_UCHAR date_time[20] = "    :  :     :  :  ";

	g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.LValue = jpg_data->exif_info.width;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.LValue = jpg_data->exif_info.lines;
	g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormatLength.ValueOffset.LValue = jpg_data->thumb_size;

	static char model[FJ_CUSTOM_EXIF_MODEL_S] = {0};
	static float focal_length = 0;
	Cate_Get_Exif_focal_length(&focal_length);
	sprintf(model, "FT1-26%d", (int)focal_length);
	memcpy(g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueModel, 
				model,
				FJ_CUSTOM_EXIF_MODEL_S);
	printf("MODEL = %s\n", g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueModel);
	// DateTimeOriginal
#ifdef CO_BF_FS_USER_CUSTOM_USE_BCD_DATE
	dcf_exif_set_bcd_datetime((FW_UCHAR*)date_time, jpg_data->exif_info.date);
#else
	dcf_exif_set_bin_datetime((FW_UCHAR*)date_time, jpg_data->exif_info.date);
#endif
	memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeOriginal, date_time, 20);

	// DateTimeDigitized
	memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeDigitized, date_time, 20);

	// DateTime
	memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueDateTime, date_time, 20);

	// DateTime1
	memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1_value.ValueDateTime1, date_time, 20);

	g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Count = jpg_data->exif_info.maker_note_length;

	g_bf_dcf_exif_jpeg_header_first.Ifd0.offset_to_1stIFD += jpg_data->exif_info.maker_note_length;
#ifdef CO_BF_DCF_IF_SET_GPSTAG
	// Latitude
	g_bf_dcf_exif_jpeg_header_second.Gps.GPSLatitudeRef.ValueOffset.LValue = 0;       // to clear UValue[ 1 -3 ]
	g_bf_dcf_exif_jpeg_header_second.Gps.GPSLatitudeRef.ValueOffset.UValue[0] = jpg_data->exif_info.gps.latitude_ref;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLatitude[0].Num = jpg_data->exif_info.gps.latitude[0].nume;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLatitude[0].Den = jpg_data->exif_info.gps.latitude[0].denomi;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLatitude[1].Num = jpg_data->exif_info.gps.latitude[1].nume;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLatitude[1].Den = jpg_data->exif_info.gps.latitude[1].denomi;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLatitude[2].Num = jpg_data->exif_info.gps.latitude[2].nume;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLatitude[2].Den = jpg_data->exif_info.gps.latitude[2].denomi;
	
	// Longitude
	g_bf_dcf_exif_jpeg_header_second.Gps.GPSLongitudeRef.ValueOffset.LValue = 0;      // to clear UValue[ 1 -3 ]
	g_bf_dcf_exif_jpeg_header_second.Gps.GPSLongitudeRef.ValueOffset.UValue[0] = jpg_data->exif_info.gps.longitude_ref;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLongitude[0].Num = jpg_data->exif_info.gps.longitude[0].nume;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLongitude[0].Den = jpg_data->exif_info.gps.longitude[0].denomi;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLongitude[1].Num = jpg_data->exif_info.gps.longitude[1].nume;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLongitude[1].Den = jpg_data->exif_info.gps.longitude[1].denomi;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLongitude[2].Num = jpg_data->exif_info.gps.longitude[2].nume;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLongitude[2].Den = jpg_data->exif_info.gps.longitude[2].denomi;
	
	// Image direction
	g_bf_dcf_exif_jpeg_header_second.Gps.GPSImgDirectionRef.ValueOffset.LValue = 0;      // to clear UValue[ 1 -3 ]
	g_bf_dcf_exif_jpeg_header_second.Gps.GPSImgDirectionRef.ValueOffset.UValue[0] = jpg_data->exif_info.gps.img_direction_ref;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSImgDirection.Num = jpg_data->exif_info.gps.img_direction.nume;
	g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSImgDirection.Den = jpg_data->exif_info.gps.img_direction.denomi;

	g_bf_dcf_exif_jpeg_header_first.Ifd0.GpsIFDPointer.ValueOffset.LValue += jpg_data->exif_info.maker_note_length;
	g_bf_dcf_exif_jpeg_header_second.Gps.GPSLatitude.ValueOffset.Offset += jpg_data->exif_info.maker_note_length;
	g_bf_dcf_exif_jpeg_header_second.Gps.GPSLongitude.ValueOffset.Offset += jpg_data->exif_info.maker_note_length;
	g_bf_dcf_exif_jpeg_header_second.Gps.GPSImgDirection.ValueOffset.Offset += jpg_data->exif_info.maker_note_length;
#endif
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer.ValueOffset.LValue += jpg_data->exif_info.maker_note_length;
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	g_bf_dcf_exif_jpeg_header_second.Ifd1.Maker1.ValueOffset.Offset += jpg_data->exif_info.maker_note_length;
	g_bf_dcf_exif_jpeg_header_second.Ifd1.Model1.ValueOffset.Offset += jpg_data->exif_info.maker_note_length;
#endif
	g_bf_dcf_exif_jpeg_header_second.Ifd1.XResolution.ValueOffset.Offset += jpg_data->exif_info.maker_note_length;
	g_bf_dcf_exif_jpeg_header_second.Ifd1.YResolution.ValueOffset.Offset += jpg_data->exif_info.maker_note_length;
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	g_bf_dcf_exif_jpeg_header_second.Ifd1.DateTime1.ValueOffset.Offset += jpg_data->exif_info.maker_note_length;
#endif
	g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormat.ValueOffset.LValue += jpg_data->exif_info.maker_note_length;

	return ercd;
}

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @brief set exif of mpo.
 * @param i:thumb_size thumbnail data size
 * @param i:main_size main data size
 * @param i:vga_size vga data size
 * @param i:exif_info setting exif information
 * @param o:*jpg_head the pointer of jpeg header
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Exif_Make_Ext(T_BF_DCF_IF_EXT_MP_DATA* ext_data)
{
	FW_INT32 ercd = 0;
	FW_UCHAR date_time[20] = "    :  :     :  :  ";

	g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.LValue = ext_data->exif_info.width;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.LValue = ext_data->exif_info.lines;
	g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormatLength.ValueOffset.LValue = ext_data->thumb_size;

	// DateTimeOriginal
#ifdef CO_BF_FS_USER_CUSTOM_USE_BCD_DATE
	dcf_exif_set_bcd_datetime((FW_UCHAR*)date_time, ext_data->exif_info.date);
#else
	dcf_exif_set_bin_datetime((FW_UCHAR*)date_time, ext_data->exif_info.date);
#endif
	memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeOriginal, date_time, 20);

	// DateTimeDigitized
	memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeDigitized, date_time, 20);

	// DateTime
	memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueDateTime, date_time, 20);

	// DateTime1
	memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1_value.ValueDateTime1, date_time, 20);

	g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Count = ext_data->exif_info.maker_note_length;

	g_bf_dcf_exif_jpeg_header_first.Ifd0.offset_to_1stIFD += ext_data->exif_info.maker_note_length;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer.ValueOffset.LValue += ext_data->exif_info.maker_note_length;
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	g_bf_dcf_exif_jpeg_header_second.Ifd1.Maker1.ValueOffset.Offset += ext_data->exif_info.maker_note_length;
	g_bf_dcf_exif_jpeg_header_second.Ifd1.Model1.ValueOffset.Offset += ext_data->exif_info.maker_note_length;
#endif
	g_bf_dcf_exif_jpeg_header_second.Ifd1.XResolution.ValueOffset.Offset += ext_data->exif_info.maker_note_length;
	g_bf_dcf_exif_jpeg_header_second.Ifd1.YResolution.ValueOffset.Offset += ext_data->exif_info.maker_note_length;
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	g_bf_dcf_exif_jpeg_header_second.Ifd1.DateTime1.ValueOffset.Offset += ext_data->exif_info.maker_note_length;
#endif
	g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormat.ValueOffset.LValue += ext_data->exif_info.maker_note_length;

	return ercd;
}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

/**
 * @brief set exif of THM.
 * @param i:thumb_size thumbnail data size
 * @param i:main_size main data size
 * @param i:vga_size vga data size
 * @param i:exif_info setting exif information
 * @param o:*jpg_head the pointer of jpeg header
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Exif_Make_Thm(T_BF_DCF_IF_THM_DATA* thm_data)
{
	FW_INT32 ercd = 0;
	FW_UCHAR date_time[20] = "    :  :     :  :  ";

	g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.LValue = thm_data->exif_info.width;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.LValue = thm_data->exif_info.lines;

	// DateTimeOriginal
#ifdef CO_BF_FS_USER_CUSTOM_USE_BCD_DATE
	dcf_exif_set_bcd_datetime((FW_UCHAR*)date_time, thm_data->exif_info.date);
#else
	dcf_exif_set_bin_datetime((FW_UCHAR*)date_time, thm_data->exif_info.date);
#endif
	memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeOriginal, date_time, 20);

	// DateTimeDigitized
	memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeDigitized, date_time, 20);

	// DateTime
	memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueDateTime, date_time, 20);

	return ercd;
}

/**
 * @brief analyze all exif tag offset in JPEG.
 * @param i:file_id file id
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Exif_Analyze_Jpeg_Resize(FW_INT32 file_id, FW_CHAR copy_flag, T_BF_DCF_EXIF_IMAGE_SIZE_INFO* exif_image_size_info)
{
	FW_INT32  ercd, ercd2;
	FW_UCHAR* buffer;
	FW_ULONG  result;
	FW_USHORT app1_size = 0;
	FW_ULONG  zero_ifd_num = 0;
	FW_ULONG  next_ifd_ptr = 0;
	FW_ULONG  first_ifd_offset = 0;
	FW_ULONG  tag_offset = 0;
	FW_ULONG  reset_offset = 0;
	FW_ULONG  value_offset = 0;
	FW_ULONG  total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
	FW_UCHAR  tag_type;
	FW_ULONG  tag_count;
	FW_ULONG  data_offset;
	FW_USHORT tmp_value;
	FW_USHORT app_zero_size = 0;
	FW_USHORT jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
	FW_USHORT zero_ifd_offset = D_BF_DCF_EXIF_TIFFHEADSIZE;
#ifdef CO_BF_DCF_IF_MPF_ENABLE
	FW_ULONG  mp_tag_offset = 0;
	FW_ULONG  mp_head_offset;
	FW_ULONG  mp_entry_info;
	FW_USHORT mp_entry_num;
	FW_USHORT mp_entry_cnt;
#endif	// CO_BF_DCF_IF_MPF_ENABLE

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
#endif

#ifdef CO_BF_DCF_IF_MPF_ENABLE
	exif_image_size_info->class1_offset = 0;
	exif_image_size_info->class2_offset = 0;
#endif

	// memory allocation
	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&buffer);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	while (1) {
		// read data from Exif
		// file pointer : file top
		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		if (*(buffer + 3) == 0xDB) {
		// check DQT marker
			// detect JPEG(without APPx) format
			break;
		}
		else if (*(buffer + 3) == 0xE0) {
		// check APP0 marker
			app_zero_size = dcf_exif_get_short_value_big_endian(buffer + 4) + 2;
			jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET + app_zero_size;
		}
		else {
			; // DO NOTHING
		}

		if (*(buffer + app_zero_size + 3) == 0xDB) {
		// check DQT marker
			// detect JFIF(no thumbnail) format
			break;
		}
		else if (*(buffer + app_zero_size + 3) == 0xE0) {
		// check APP0 marker
			// detect JFIF(JFXX) format
			break;

		}
		else if (*(buffer + app_zero_size + 3) != 0xE1) {
		// check APP1 marker
		// APP1 marker's place is next to SOI marker
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND APP1 MARKER\n"));
#endif
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;
		}
		else {
			; // DO NOTHING
		}

		app1_size = dcf_exif_get_short_value_big_endian((buffer + app_zero_size + 4));

		// reset pointer
		ercd = BF_Mdf_Wrapper_Lseek(file_id, (4 + app_zero_size + app1_size), SEEK_SET, &result);
		if (ercd != 0) {
			// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
			// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		if (*(buffer + 1) == 0xE2) {
#ifdef CO_BF_DCF_IF_MPF_ENABLE
			if (strcmp((const FW_CHAR*)(buffer + 4), "MPF") == 0) {
				// MPF format
				mp_head_offset = (4 + app1_size + 8);
				exif_image_size_info->app2_size = dcf_exif_get_short_value_big_endian(buffer + 2) + 2;

				g_bf_dcf_exif_big_or_lit = *(FW_USHORT*)(buffer + 8);

				// offset of next IFD + APP2 marker(2Byte) + APP2 length(2Byte) + identification code(4Byte) + tag count(2Byte)
				mp_tag_offset = dcf_exif_get_long_value(buffer + 12) + 10;
				gNumOfTag = dcf_exif_get_short_value(buffer + mp_tag_offset - 2);	// Num of MP index IFD's tag
				gTagCheckCnt = 1;	// reset seearch count

				// Check MP Entry Num
				ercd = dcf_exif_check_exif_tag(file_id, buffer, 0xB001, &mp_tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
				if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
					// MP Entry num not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND MP ENTRY NUM\n"));
#endif
					break;
				}
				else if (ercd != 0) {
					// error
					break;
				}
				else {
					// get MP entry num
					mp_entry_num = dcf_exif_get_long_value(buffer + mp_tag_offset + value_offset);
				}
				tag_offset = reset_offset;

				// Check MP Entry
				ercd = dcf_exif_check_exif_tag(file_id, buffer, 0xB002, &mp_tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
				if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
					// MP entry not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
					BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND MP ENTRY\n"));
#endif
					break;
				}
				else if (ercd != 0) {
					// error
					break;
				}
				else {
					// get MP entry
					data_offset = 8 + dcf_exif_get_long_value(buffer + mp_tag_offset + value_offset);

					for (mp_entry_cnt = 0; mp_entry_cnt < mp_entry_num; mp_entry_cnt++) {
						if (mp_entry_cnt >= 3) {
							// MP entry num over 3 (not Baseline MP format)
							break;
						}
						mp_entry_info = dcf_exif_get_long_value(buffer + data_offset + (16 * mp_entry_cnt));

						if ((mp_entry_info & 0x000FFFFF) == 0x00030000) {
							// Baseline Main Image
						}
						else if ((mp_entry_info & 0x000FFFFF) == 0x00010001) {
							// ScreenNail (VGA)
							exif_image_size_info->class1_offset = dcf_exif_get_long_value(buffer + data_offset + (16 * mp_entry_cnt) + 8) + mp_head_offset;
						}
						else if ((mp_entry_info & 0x000FFFFF) == 0x00010002) {
							// ScreenNail (Full HD)
							exif_image_size_info->class2_offset = dcf_exif_get_long_value(buffer + data_offset + (16 * mp_entry_cnt) + 8) + mp_head_offset;
						}
						else {
							; // DO NOTHING
						}
					}
				}
				g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
			}
			else {
				; // DO NOTHING
			}
#endif	// CO_BF_DCF_IF_MPF_ENABLE
		}

		ercd = BF_Mdf_Wrapper_Lseek(file_id, 0, D_BF_MDF_WRAPPER_SEEK_SET, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		// get filed length offset
		exif_image_size_info->field_length_offset = 4;

		// check endian
		g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));
#ifdef CO_BF_DCF_IF_MPF_ENABLE
		exif_image_size_info->endian = g_bf_dcf_exif_big_or_lit;
#endif	// CO_BF_DCF_IF_MPF_ENABLE

		zero_ifd_offset = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);
		zero_ifd_num = dcf_exif_get_short_value(buffer + jpeg_start_offset + zero_ifd_offset);
		next_ifd_ptr  = jpeg_start_offset + zero_ifd_offset + 2 + zero_ifd_num * sizeof(T_BF_DCF_EXIF_IFD);	// point Next IFD Pointer
		first_ifd_offset  = dcf_exif_get_long_value(buffer + next_ifd_ptr);

		tag_offset = jpeg_start_offset + zero_ifd_offset + 2;
		reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
		gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
		gTagCheckCnt = 1;	// reset seearch count

		// Get Orientation
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_ORIENT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND ORIENTATION\n"));
#endif
			g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation.ValueOffset.SValue.Value = E_BF_DCF_IF_ROTATE_0;
			exif_image_size_info->zero_orient_offset = 0;
		}
		else if (ercd != 0) {
			g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation.ValueOffset.SValue.Value = E_BF_DCF_IF_ROTATE_0;
			exif_image_size_info->zero_orient_offset = 0;
			break;
		}
		else {
			tmp_value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			if (tmp_value < 9) {
				g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation.ValueOffset.SValue.Value = (E_BF_DCF_IF_ROTATE)tmp_value;
			}
			else {
				g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation.ValueOffset.SValue.Value = E_BF_DCF_IF_ROTATE_0;
			}
			exif_image_size_info->zero_orient_offset = tag_offset + value_offset + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		tag_offset = reset_offset;

		// Get date & time
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_DATETIME_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EDIT DATE\n"));
#endif
			exif_image_size_info->edit_date_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->edit_date_offset = 0;
			break;
		}
		else {
			exif_image_size_info->edit_date_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset) + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
			memcpy( &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueDateTime, (void*)(exif_image_size_info->edit_date_value), D_BF_DCF_EXIF_V_DATEORIG_S - 1);
		}
		tag_offset = reset_offset;

		// Get EXIF IDF offset
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND EXIF IDF OFFSET\n"));
#endif
			break;
		}
		else if (ercd != 0) {
			break;
		}
		else {
			;
		}
		data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

		// get next start offset
		ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
		if (ercd != 0) {
			break;
		}

		// Get ExposureTime
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_EXPTIME_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EXPOSURE TIME\n"));
#endif
			exif_image_size_info->exposure_time_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->exposure_time_offset = 0;
			break;
		}
		else {
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			exif_image_size_info->exposure_time_offset = data_offset + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer,
											   (FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureTime.Num,
											   (FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureTime.Den);
			if (ercd != 0) {
				break;
			}
		}
		tag_offset = reset_offset;

		// Get F Value
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_FNUM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND F_VALUE\n"));
#endif
			exif_image_size_info->f_number_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->f_number_offset = 0;
			break;
		}
		else {
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			exif_image_size_info->f_number_offset = data_offset + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer,
											   (FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueFNumber.Num,
											   (FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueFNumber.Den);
			if (ercd != 0) {
				break;
			}
		}
		tag_offset = reset_offset;

		// ISO speed ratings
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_ISOSPEEDRATE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND ISO SPEED RATINGS\n"));
#endif
			exif_image_size_info->iso_speed_rate_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->iso_speed_rate_offset = 0;
			break;
		}
		else {
			g_bf_dcf_exif_jpeg_header_first.ExifIfd.ISOSpeedRatings.ValueOffset.SValue.Value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			exif_image_size_info->iso_speed_rate_offset = tag_offset + value_offset + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		tag_offset = reset_offset;

		// Get date & time Original
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_DATETIMEORIG_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND DATE TIME ORIGINAL\n"));
#endif
			exif_image_size_info->date_time_original_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->date_time_original_offset = 0;
			break;
		}
		else {
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			exif_image_size_info->date_time_original_offset = data_offset + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
//			ercd = dcf_exif_get_exif_date_value(file_id, buffer, &data_offset, total_buffer, &img_detail->DateTimeOriginal);
			memcpy( &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeOriginal, (void*)(buffer + data_offset), D_BF_DCF_EXIF_V_DATEORIG_S - 1);
		}
		tag_offset = reset_offset;

		// Get date & time Digital
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_DATETIMEDITI_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND DATE TIME DIGITAL\n"));
#endif
			exif_image_size_info->date_time_digit_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->date_time_digit_offset = 0;
			break;
		}
		else {
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			exif_image_size_info->date_time_digit_offset = data_offset + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
//			ercd = dcf_exif_get_exif_date_value(file_id, buffer, &data_offset, total_buffer, &img_detail->DateTimeDigital);
			memcpy( &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeDigitized, (void*)(buffer + data_offset), D_BF_DCF_EXIF_V_DATEORIG_S - 1);
		}
		tag_offset = reset_offset;

		// Exposure bias
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_EXPBIAS_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EXPOSURE_BIAS\n"));
#endif
			exif_image_size_info->exposure_bias_value_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->exposure_bias_value_offset = 0;
			break;
		}
		else {
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			exif_image_size_info->exposure_bias_value_offset = data_offset + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
#if 0
			ercd = dcf_exif_get_exif_set_value_ex(file_id, buffer, &data_offset, total_buffer,
												  (FW_LONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureBiasValue.Num,
												  (FW_LONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureBiasValue.Den);
#endif
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer,
											   (FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureBiasValue.Num,
											   (FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureBiasValue.Den);
			if (ercd != 0) {
				break;
			}
		}
		tag_offset = reset_offset;

		// Get MeteringMode
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_METERMODE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND METERING_MODE\n"));
#endif
			exif_image_size_info->metering_mode_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->metering_mode_offset = 0;
			break;
		}
		else {
			g_bf_dcf_exif_jpeg_header_first.ExifIfd.MeteringMode.ValueOffset.SValue.Value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			exif_image_size_info->metering_mode_offset = tag_offset + value_offset + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		tag_offset = reset_offset;

		// flash
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_FLASH_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND FLASH\n"));
#endif
			exif_image_size_info->flash_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->flash_offset = 0;
			break;
		}
		else {
			g_bf_dcf_exif_jpeg_header_first.ExifIfd.Flash.ValueOffset.SValue.Value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			exif_image_size_info->flash_offset = tag_offset + value_offset + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		tag_offset = reset_offset;

#if 0
		// maker note
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_MAKERNOTE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND MAKERNOTE\n"));
#endif
		}
		else if (ercd != 0) {
			break;
		}
		else {
			g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.ValueOffset.Offset = (FW_ULONG)(D_BF_DCF_EXIF_TIFFOFFSET + dcf_exif_get_long_value(buffer + tag_offset + value_offset));
			g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Count = tag_count;
		}
		tag_offset = reset_offset;
#endif // 0

		// image width
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELXDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_X_DIMENSION\n"));
#endif
			exif_image_size_info->x_size_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->x_size_offset = 0;
			break;
		}
		else {
//			if(tag_type == 3) {
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.SValue.Value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.SValue.Value = (FW_USHORT)exif_image_size_info->x_size_value;
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.SValue.dummy = 0;
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.Type = D_BF_DCF_EXIF_SHORT_T;
//			}
//			else {
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.LValue = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.LValue = exif_image_size_info->x_size_value;
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.Type = D_BF_DCF_EXIF_LONG_T;
//			}
			exif_image_size_info->x_size_offset = tag_offset + value_offset + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
			g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.LValue = exif_image_size_info->x_size_value;
			g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.Type = D_BF_DCF_EXIF_LONG_T;
		}
		tag_offset = reset_offset;

		// image height
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELYDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_Y_DIMENSION\n"));
#endif
			exif_image_size_info->y_size_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->y_size_offset = 0;
			break;
		}
		else {
//			if(tag_type == 3) {
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.SValue.Value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.SValue.Value = (FW_USHORT)exif_image_size_info->y_size_value;
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.SValue.dummy = 0;
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.Type = D_BF_DCF_EXIF_SHORT_T;
//			}
//			else {
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.LValue = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.LValue = exif_image_size_info->y_size_value;
//				g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.Type = D_BF_DCF_EXIF_LONG_T;
//			}
			exif_image_size_info->y_size_offset = tag_offset + value_offset + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
			g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.LValue = exif_image_size_info->y_size_value;
			g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.Type = D_BF_DCF_EXIF_LONG_T;
		}
		tag_offset = reset_offset;

		// Get WhiteBalanceMode
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_WB_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND WHITE_BALANCE_MODE\n"));
#endif
			exif_image_size_info->white_balance_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->white_balance_offset = 0;
			break;
		}
		else {
			g_bf_dcf_exif_jpeg_header_first.ExifIfd.WhiteBalanceMode.ValueOffset.SValue.Value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			exif_image_size_info->white_balance_offset = tag_offset + value_offset + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		tag_offset = reset_offset;

		// get NEXT IFD Pointer
		ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + first_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
		if (ercd != 0) {
			break;
		}

		// Get Orientation
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_ORIENT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND ORIENTATION thumb\n"));
#endif
			exif_image_size_info->first_orient_offset = 0;
			g_bf_dcf_exif_jpeg_header_second.Ifd1.Orientation.ValueOffset.SValue.Value = E_BF_DCF_IF_ROTATE_0;
		}
		else if (ercd != 0) {
			exif_image_size_info->first_orient_offset = 0;
			break;
		}
		else {
			tmp_value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			if (tmp_value < 9) {
				g_bf_dcf_exif_jpeg_header_second.Ifd1.Orientation.ValueOffset.SValue.Value = (E_BF_DCF_IF_ROTATE)tmp_value;
			}
			else {
				g_bf_dcf_exif_jpeg_header_second.Ifd1.Orientation.ValueOffset.SValue.Value = E_BF_DCF_IF_ROTATE_0;
			}
			exif_image_size_info->first_orient_offset = tag_offset + value_offset + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		tag_offset = reset_offset;

		// Get date & time(thumbnail)
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_DATETIME1_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EDIT DATE thumb\n"));
#endif
			exif_image_size_info->edit_date_thumb_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->edit_date_thumb_offset = 0;
			break;
		}
		else {
			exif_image_size_info->edit_date_thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset) + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
//			memcpy( &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueDateTime, (void *)(exif_image_size_info->edit_date_value), D_BF_DCF_EXIF_V_DATEORIG_S - 1);
		}
		tag_offset = reset_offset;

		// search JPEG InterchangeFormat IFD (0x201)
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND JPEG INTER_CHANGE_FORMAT\n"));
#endif
			exif_image_size_info->thumb_offset = 0;
			exif_image_size_info->field_length_offset = 0;
			break;
		}
		else if (ercd != 0) {
			exif_image_size_info->thumb_offset = 0;
			break;
		}
		else {
			;
		}
		exif_image_size_info->thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

		// JPEG InterchangeFormatLength tag is 0x202
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND JPEG INTER_CHANGE_FORMAT_LENGTH\n"));
#endif
			exif_image_size_info->field_length_offset = 0;
			break;
		}
		else if (ercd != 0) {
			exif_image_size_info->field_length_offset = 0;
			break;
		}
		else {
			;
		}
		exif_image_size_info->thumb_length_offset = (tag_offset + value_offset) + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		break;
	}

	// make field length value
	if (copy_flag) {
		exif_image_size_info->field_length_value = app1_size;
#if 1
		if (exif_image_size_info->thumb_length_value == 0) {
			exif_image_size_info->thumb_length_value = app1_size + D_BF_DCF_EXIF_SOI_SIZE - exif_image_size_info->thumb_offset;
		}
#endif
	}
	else {
		if (exif_image_size_info->thumb_offset == 0) {
			exif_image_size_info->field_length_value = app1_size;
		}
		else {
			exif_image_size_info->field_length_value = exif_image_size_info->thumb_offset + exif_image_size_info->thumb_length_value - D_BF_DCF_EXIF_SOI_SIZE;
		}
	}

	ercd2 = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)buffer);
	if (ercd2 != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd2));
#endif
		ercd = ercd2;
	}

	// reset file pointer
	ercd2 = BF_Mdf_Wrapper_Lseek(file_id, 0, D_BF_MDF_WRAPPER_SEEK_SET, &result);
	if (ercd2 != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		ercd = ercd2;
	}

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	if (g_bf_dcf_exif_big_or_lit == D_BF_DCF_EXIF_LIT_EN) {
		// make thumb_size value
		dcf_exif_swap_long(&exif_image_size_info->thumb_length_value);
	}
#else
	if (g_bf_dcf_exif_big_or_lit == D_BF_DCF_EXIF_BIG_EN) {
		// make thumb_size value
		dcf_exif_swap_long(&exif_image_size_info->thumb_length_value);
	}
#endif

	return ercd;
}

static FW_VOID dcf_exif_exchange_endian_resize(FW_VOID)
{
	// Change Endian
	// make 0th orientation value
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation.ValueOffset.SValue.Value);

	// make exposure time value
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureTime.Num);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureTime.Den);

	// make f number value
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueFNumber.Num);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueFNumber.Den);

	// make iso speed rating value
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ISOSpeedRatings.ValueOffset.SValue.Value);

	// make exposure bias value
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureBiasValue.Num);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureBiasValue.Den);

	// make metering mode value
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.MeteringMode.ValueOffset.SValue.Value);

	// make flash value
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Flash.ValueOffset.SValue.Value);

	// make pixel x value
	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.Type == D_BF_DCF_EXIF_SHORT_T) {
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.SValue.Value);
	}
	else {
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.LValue);
	}
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.Type);

	// make pixel y value
	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.Type == D_BF_DCF_EXIF_SHORT_T) {
		dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.SValue.Value);
	}
	else {
		dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.LValue);
	}
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.Type);

	// make white balance mode value
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.WhiteBalanceMode.ValueOffset.SValue.Value);

	// make 1st orientation value
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.Orientation.ValueOffset.SValue.Value);
}

/**
 * @brief reset exif data of used resize for capture mode.
 * @param none
 * @return error code
 * @note none
 * @attention none.
 */
//static FW_VOID reset_resize_exif_value(FW_VOID)
FW_INT32 BF_Dcf_Exif_Reset_Jpeg_Resize(FW_VOID)
{
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation.ValueOffset.SValue.Value = E_BF_DCF_IF_ROTATE_0;
	memcpy((FW_CHAR*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueDateTime, "    :  :     :  :  ", D_BF_DCF_EXIF_V_DATETIME_S);
	g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureTime.Num = 1;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureTime.Den = 60;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueFNumber.Num = 29;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueFNumber.Den = 10;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.ISOSpeedRatings.ValueOffset.SValue.Value = 0x0064;
	memcpy((FW_CHAR*)g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeOriginal, "    :  :     :  :  ", D_BF_DCF_EXIF_V_DATEORIG_S);
	memcpy((FW_CHAR*)g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeDigitized, "    :  :     :  :  ", D_BF_DCF_EXIF_V_DATEORIG_S);
	g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureBiasValue.Num = 0;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureBiasValue.Den = 10;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.MeteringMode.ValueOffset.SValue.Value = 0x0000;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.Flash.ValueOffset.SValue.Value = 0x0000;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.LValue = 0x00000000;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.Type = D_BF_DCF_EXIF_LONG_T;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.LValue = 0x00000000;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.Type = D_BF_DCF_EXIF_LONG_T;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.WhiteBalanceMode.ValueOffset.SValue.Value = 0x0000;
	g_bf_dcf_exif_jpeg_header_second.Ifd1.Orientation.ValueOffset.SValue.Value = E_BF_DCF_IF_ROTATE_0;

	return 0;
}

/**
 * @brief Resize save Exif data copy & tag value set.
 * @param i: src_file_id copy source file ID
 * @param i: dst_file_id copy destination file ID
 * @param i: *exif_image_size_info set exif value offset
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Exif_Copy_Jpeg_Resize(FW_INT32 src_file_id, FW_INT32 dst_file_id, T_BF_DCF_EXIF_IMAGE_SIZE_INFO* exif_image_size_info)
{
	FW_UCHAR* buffer;
	FW_ULONG  total_exif_size = 0;
	FW_ULONG  result = 0;
	FW_INT32  ercd;
	FW_ULONG  exif_size;
	FW_USHORT write_app1_size;

	// memory allocation
	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&buffer);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	if (g_bf_dcf_exif_big_or_lit == D_BF_DCF_EXIF_LIT_EN) {
		dcf_exif_exchange_endian_resize();
	}
#else
	if (g_bf_dcf_exif_big_or_lit == D_BF_DCF_EXIF_BIG_EN) {
		dcf_exif_exchange_endian_resize();
	}
#endif

	if (exif_image_size_info->thumb_offset == 0) {
		exif_size = exif_image_size_info->field_length_value;
	}
	else {
		exif_size = exif_image_size_info->thumb_offset;
	}

	// exif data copy
//	while(total_exif_size < exif_image_size_info->thumb_offset){
	while(total_exif_size < exif_size){
		ercd = BF_Mdf_Wrapper_Read(src_file_id, (FW_VOID*)buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, src_file_id));
#endif
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)buffer);
			return ercd;
		}

		while (1) {
			// field length value set
			if (exif_image_size_info->field_length_offset == 0) {
				;
			}
			else if (exif_image_size_info->field_length_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->field_length_offset >= total_exif_size) {
				write_app1_size = BF_Dcf_Exif_Exchange_App_Size(exif_image_size_info->field_length_value);
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->field_length_offset - total_exif_size), &write_app1_size, sizeof(FW_USHORT));
			}
			else {
				; // DO NOTHING
			}

		// 0th IFD
			// 0th orientation set
			if (exif_image_size_info->zero_orient_offset == 0) {
				;
			}
			else if (exif_image_size_info->zero_orient_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->zero_orient_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->zero_orient_offset - total_exif_size), (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation.ValueOffset.SValue.Value, sizeof(FW_USHORT));
			}
			else {
				; // DO NOTHING
			}

		// 0th IFD value
			// edit date time set
			if (exif_image_size_info->edit_date_offset == 0) {
				;
			}
			else if (exif_image_size_info->edit_date_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->edit_date_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->edit_date_offset - total_exif_size), (FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueDateTime, D_BF_DCF_EXIF_V_DATETIME_S);
			}
			else {
				; // DO NOTHING
			}

		// Exif IFD
			// iso speed rating set
			if (exif_image_size_info->iso_speed_rate_offset == 0) {
				;
			}
			else if (exif_image_size_info->iso_speed_rate_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->iso_speed_rate_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->iso_speed_rate_offset - total_exif_size), (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ISOSpeedRatings.ValueOffset.SValue.Value, sizeof(FW_USHORT));
			}
			else {
				; // DO NOTHING
			}

			// metering mode set
			if (exif_image_size_info->metering_mode_offset == 0) {
				;
			}
			else if (exif_image_size_info->metering_mode_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->metering_mode_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->metering_mode_offset - total_exif_size), (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.MeteringMode.ValueOffset.SValue.Value, sizeof(FW_USHORT));
			}
			else {
				; // DO NOTHING
			}

			// flash set
			if (exif_image_size_info->flash_offset == 0) {
				;
			}
			else if (exif_image_size_info->flash_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->flash_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->flash_offset - total_exif_size), (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Flash.ValueOffset.SValue.Value, sizeof(FW_USHORT));
			}
			else {
				; // DO NOTHING
			}

			// pixel x dimension set
			if (exif_image_size_info->x_size_offset == 0) {
				;
			}
			else if (exif_image_size_info->x_size_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->x_size_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->x_size_offset - total_exif_size - 6), (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.Type, sizeof(FW_USHORT));
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->x_size_offset - total_exif_size), (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.LValue, sizeof(FW_ULONG));
			}
			else {
				; // DO NOTHING
			}

			// pixel y dimension set
			if (exif_image_size_info->y_size_offset == 0) {
				;
			}
			else if (exif_image_size_info->y_size_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->y_size_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->y_size_offset - total_exif_size - 6), (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.Type, sizeof(FW_USHORT));
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->y_size_offset - total_exif_size), (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.LValue, sizeof(FW_ULONG));
			}
			else {
				; // DO NOTHING
			}

			// white balance set
			if (exif_image_size_info->white_balance_offset == 0) {
				;
			}
			else if (exif_image_size_info->white_balance_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->white_balance_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->white_balance_offset - total_exif_size), (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.WhiteBalanceMode.ValueOffset.SValue.Value, sizeof(FW_USHORT));
			}
			else {
				; // DO NOTHING
			}

		// Exif IFD value
			// exposure time set
			if (exif_image_size_info->exposure_time_offset == 0) {
				;
			}
			else if (exif_image_size_info->exposure_time_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->exposure_time_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->exposure_time_offset - total_exif_size), (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureTime, sizeof(T_BF_DCF_EXIF_RATIONAL));
			}
			else {
				; // DO NOTHING
			}

			// f number set
			if (exif_image_size_info->f_number_offset == 0) {
				;
			}
			else if (exif_image_size_info->f_number_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->f_number_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->f_number_offset - total_exif_size), (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueFNumber, sizeof(T_BF_DCF_EXIF_RATIONAL));
			}
			else {
				; // DO NOTHING
			}

			// date time original set
			if (exif_image_size_info->date_time_original_offset == 0) {
				;
			}
			else if (exif_image_size_info->date_time_original_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->date_time_original_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->date_time_original_offset - total_exif_size), (FW_VOID*)g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeOriginal, D_BF_DCF_EXIF_V_DATEORIG_S);
			}
			else {
				; // DO NOTHING
			}

			// date time digitized set
			if (exif_image_size_info->date_time_digit_offset == 0) {
				;
			}
			else if (exif_image_size_info->date_time_digit_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->date_time_digit_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->date_time_digit_offset - total_exif_size), (FW_VOID*)g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeDigitized, D_BF_DCF_EXIF_V_DATEORIG_S);
			}
			else {
				; // DO NOTHING
			}

			// exposure bias value set
			if (exif_image_size_info->exposure_bias_value_offset == 0) {
				;
			}
			else if (exif_image_size_info->exposure_bias_value_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->exposure_bias_value_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->exposure_bias_value_offset - total_exif_size), (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureBiasValue, sizeof(T_BF_DCF_EXIF_SRATIONAL));
			}
			else {
				; // DO NOTHING
			}

		// 1st IFD
			// 1st IFD orientation set
			if (exif_image_size_info->first_orient_offset == 0) {
				;
			}
			else if (exif_image_size_info->first_orient_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->first_orient_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->first_orient_offset - total_exif_size), (FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.Orientation.ValueOffset.SValue.Value, sizeof(FW_USHORT));
			}
			else {
				; // DO NOTHING
			}

			// 1st IFD thumbnail offset set
			if (exif_image_size_info->thumb_length_offset == 0) {
				;
			}
			else if (exif_image_size_info->thumb_length_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->thumb_length_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->thumb_length_offset - total_exif_size), &exif_image_size_info->thumb_length_value, sizeof(FW_ULONG));
			}
			else {
				; // DO NOTHING
			}

		// 1st IFD value
			// edit date time set
			if (exif_image_size_info->edit_date_thumb_offset == 0) {
				;
			}
			else if (exif_image_size_info->edit_date_thumb_offset >= (total_exif_size + D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE)) {
				break;
			}
			else if (exif_image_size_info->edit_date_thumb_offset >= total_exif_size) {
				memcpy((FW_CHAR*)(buffer + exif_image_size_info->edit_date_thumb_offset - total_exif_size), (FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueDateTime, D_BF_DCF_EXIF_V_DATETIME_S);
			}
			else {
				; // DO NOTHING
			}

			break;
		}

		total_exif_size += D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;

		ercd = BF_Mdf_Wrapper_Write(dst_file_id, (FW_VOID*)buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Write() failed: ercd=%d, file_id=%x\n", ercd, dst_file_id));
#endif
//			reset_resize_exif_value();
			BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)buffer);
			return ercd;
		}
	}

	/* Exif value reset (for capture mode) */
//	reset_resize_exif_value();

	/* memory free */
	ercd = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)buffer);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	return 0;
}



/**
 * @brief get thumbnail offset and vga offset.
 * @param i:file_id file id
 * @param o:*thumb_offset thumbnail offset
 * @param o:*vga_offset vga offset
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Exif_Get_Image_Offset_Jpeg(FW_INT32 file_id, FW_ULONG* main_offset, FW_ULONG* thumb_offset, FW_ULONG* vga_offset)
{
	FW_INT32  ercd, ercd2;
	FW_UCHAR* buffer;
	FW_ULONG  result;
	FW_ULONG  zero_ifd_num = 0;
	FW_ULONG  next_ifd_ptr = 0;
	FW_ULONG  first_ifd_offset = 0;
	FW_ULONG  tag_offset = 0;
	FW_ULONG  reset_offset = 0;
	FW_ULONG  value_offset = 0;
	FW_ULONG  total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
	FW_UCHAR  tag_type;
	FW_ULONG  tag_count;
	FW_ULONG  data_offset;
	FW_USHORT field_length;
	FW_USHORT app_zero_size = 0;
	FW_USHORT jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
	FW_USHORT zero_ifd_offset = D_BF_DCF_EXIF_TIFFHEADSIZE;

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
#endif

	if (vga_offset != NULL) {
		*vga_offset = 0;
	}

	// memory allocation
	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&buffer);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	while (1) {
		// read data from Exif
		// file pointer : file top
		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		// check APP0 marker
		if (*(buffer + 3) == 0xE0) {
			app_zero_size = dcf_exif_get_short_value_big_endian(buffer + 4) + 2;
			jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET + app_zero_size;
		}

		// search APP1 marker
		// APP1 marker' place is next to SOI marker
		if (*(buffer + app_zero_size + 3) != 0xE1) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND APP1 MARKER\n"));
#endif
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;
		}

		if (main_offset != NULL) {
			// get field length
			field_length = *((FW_USHORT*)(buffer + 4));
			*main_offset = field_length + 4;
		}

		if ((thumb_offset == NULL) && (vga_offset == NULL)) {
			break;
		}

		// check endian
		g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));

		zero_ifd_offset = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);
		zero_ifd_num = dcf_exif_get_short_value(buffer + jpeg_start_offset + zero_ifd_offset);
		next_ifd_ptr  = jpeg_start_offset + zero_ifd_offset + 2 + zero_ifd_num * sizeof(T_BF_DCF_EXIF_IFD);	// point Next IFD Pointer
		first_ifd_offset  = dcf_exif_get_long_value(buffer + next_ifd_ptr);

		tag_offset = jpeg_start_offset + zero_ifd_offset + 2;
		reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
		gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
		gTagCheckCnt = 1;	// reset seearch count

		// Get EXIF IDF offset
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND EXIF IDF OFFSET\n"));
#endif
			break;
		}
		else if (ercd != 0) {
			break;
		}
		else {
			; // DO NOTHING
		}
		data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
		// get next start offset
		ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
		if (ercd != 0) {
			break;
		}

		if (thumb_offset == NULL) {
			break;
		}

		// get NEXT IFD Pointer
		ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + first_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
		if (ercd != 0) {
			break;
		}

		// search JPEG InterchangeFormat IFD (0x201)
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND JPEG INTER_CHANGE_FORMAT\n"));
#endif
			break;
		}
		else if (ercd != 0) {
			break;
		}
		else {
			; // DO NOTHING
		}
		*thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
		break;
	}

	ercd2 = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)buffer);
	if (ercd2 != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd2));
#endif
		ercd = ercd2;
	}

	return ercd;
}

/**
 * @brief get exif orientation offset in JPEG.
 * @param i:file_id file id
 * @param o:*exif_orient_info exif orientation setting information
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Exif_Get_Orientation_Offset_Jpeg(FW_INT32 file_id, T_BF_DCF_EXIF_ORIENTATION_INFO* exif_orient_info)
{
	FW_INT32  ercd, ercd2;
	FW_UCHAR* buffer;
	FW_ULONG  result;
	FW_ULONG  zero_ifd_num = 0;
	FW_ULONG  next_ifd_ptr = 0;
	FW_ULONG  first_ifd_offset = 0;
	FW_ULONG  tag_offset = 0;
	FW_ULONG  reset_offset = 0;
	FW_ULONG  value_offset = 0;
	FW_ULONG  total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
	FW_UCHAR  tag_type;
	FW_ULONG  tag_count;
	FW_USHORT tmp_value;
	FW_USHORT app_zero_size = 0;
	FW_USHORT jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
	FW_USHORT zero_ifd_offset = D_BF_DCF_EXIF_TIFFHEADSIZE;

	if (exif_orient_info == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
#endif

	// memory allocation
	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&buffer);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	while (1) {
		// read data from Exif
		// file pointer : file top
		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		// check APP0 marker
		if (*(buffer + 3) == 0xE0) {
			app_zero_size = dcf_exif_get_short_value_big_endian(buffer + 4) + 2;
			jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET + app_zero_size;
		}

		// search APP1 marker
		// APP1 marker' place is next to SOI marker
		if (*(buffer + app_zero_size + 3) != 0xE1) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND APP1 MARKER\n"));
#endif
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;
		}

		// check endian
		g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));

		// make orientation value
		tmp_value = exif_orient_info->orient_value;
		exif_orient_info->orient_value = dcf_exif_get_short_value((FW_UCHAR*)&tmp_value);
#if 1
		tmp_value = exif_orient_info->first_orient_value;
		exif_orient_info->first_orient_value = dcf_exif_get_short_value((FW_UCHAR*)&tmp_value);
#endif

		zero_ifd_offset = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);
		zero_ifd_num = dcf_exif_get_short_value(buffer + jpeg_start_offset + zero_ifd_offset);
		next_ifd_ptr  = jpeg_start_offset + zero_ifd_offset + 2 + zero_ifd_num * sizeof(T_BF_DCF_EXIF_IFD);	// point Next IFD Pointer
		first_ifd_offset  = dcf_exif_get_long_value(buffer + next_ifd_ptr);

		tag_offset = jpeg_start_offset + zero_ifd_offset + 2;
		reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
		gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
		gTagCheckCnt = 1;	// reset seearch count

		// Get Orientation
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_ORIENT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND ORIENTATION\n"));
#endif
			exif_orient_info->zero_orient_offset = 0;
		}
		else if (ercd != 0) {
			exif_orient_info->zero_orient_offset = 0;
			break;
		}
		else {
			exif_orient_info->zero_orient_offset = (tag_offset + value_offset) + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		tag_offset = reset_offset;

		// Get date & time
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_DATETIME_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EDIT DATE\n"));
#endif
			exif_orient_info->edit_date_offet = 0;
		}
		else if (ercd != 0) {
			exif_orient_info->edit_date_offet = 0;
			break;
		}
		else {
			exif_orient_info->edit_date_offet = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
		}
		tag_offset = reset_offset;

		// get NEXT IFD Pointer
		ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, (jpeg_start_offset + first_ifd_offset), &tag_offset, &reset_offset, &total_buffer);
		if (ercd != 0) {
			break;
		}

		// Get Orientation
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_ORIENT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND ORIENTATION thumb\n"));
#endif
			exif_orient_info->first_orient_offet = 0;
//			ercd = 0;
		}
		else if (ercd != 0) {
			exif_orient_info->first_orient_offet = 0;
			break;
		}
		else {
			exif_orient_info->first_orient_offet = (tag_offset + value_offset) + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		tag_offset = reset_offset;

		// Get date & time
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_DATETIME1_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EDIT DATE thumb\n"));
#endif
			exif_orient_info->edit_date_offet_thumb = 0;
			ercd = 0;
		}
		else if (ercd != 0) {
			exif_orient_info->edit_date_offet_thumb = 0;
			break;
		}
		else {
			exif_orient_info->edit_date_offet_thumb = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
		}
		break;
	}

	ercd2 = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)buffer);
	if (ercd2 != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd2));
#endif
		ercd = ercd2;
	}

	return ercd;
}

/**
 * @brief get exif width&length offset in JPEG.
 * @param i:file_id file id
 * @param o:*exif_image_size_info exif x_size & y_size setting information
 * @return error code
 * @note none
 * @attention before call this function, set x_size_value,y_size_value,thumb_length_value in exif_image_size_info.\n
              (if little endian, exchange these value.)
 */
FW_INT32 BF_Dcf_Exif_Get_Image_Size_Offset_Jpeg(FW_INT32 file_id, T_BF_DCF_EXIF_IMAGE_SIZE_INFO* exif_image_size_info)
{
	FW_INT32  ercd, ercd2;
	FW_UCHAR* buffer;
	FW_ULONG  result;
	FW_ULONG  zero_ifd_num = 0;
	FW_ULONG  next_ifd_ptr = 0;
	FW_ULONG  first_ifd_offset = 0;
	FW_ULONG  tag_offset = 0;
	FW_ULONG  reset_offset = 0;
	FW_ULONG  value_offset = 0;
	FW_ULONG  data_offset = 0;
	FW_ULONG  total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
	FW_UCHAR  tag_type;
	FW_ULONG  tag_count;
	FW_USHORT app_zero_size = 0;
	FW_USHORT jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
	FW_USHORT zero_ifd_offset = D_BF_DCF_EXIF_TIFFHEADSIZE;

	if (exif_image_size_info == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
#endif

	// memory allocation
	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&buffer);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	while (1) {
		// read data from Exif
		// file pointer : file top
		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		// check APP0 marker
		if (*(buffer + 3) == 0xE0) {
			app_zero_size = dcf_exif_get_short_value_big_endian(buffer + 4) + 2;
			jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET + app_zero_size;
		}

		// search APP1 marker
		// APP1 marker' place is next to SOI marker
		if (*(buffer + app_zero_size + 3) != 0xE1) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND APP1 MARKER\n"));
#endif
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;
		}

		// get filed length offset
		exif_image_size_info->field_length_offset = 4;

		// check endian
		g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));

		zero_ifd_offset = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);
		zero_ifd_num = dcf_exif_get_short_value(buffer + jpeg_start_offset + zero_ifd_offset);
		next_ifd_ptr  = jpeg_start_offset + zero_ifd_offset + 2 + zero_ifd_num * sizeof(T_BF_DCF_EXIF_IFD);	// point Next IFD Pointer
		first_ifd_offset  = dcf_exif_get_long_value(buffer + next_ifd_ptr);

		tag_offset = jpeg_start_offset + zero_ifd_offset + 2;
		reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
		gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
		gTagCheckCnt = 1;	// reset seearch count

		// Get date & time
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_DATETIME_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND EDIT DATE\n"));
#endif
			exif_image_size_info->edit_date_offset = 0;
		}
		else if (ercd != 0) {
			exif_image_size_info->edit_date_offset = 0;
			break;
		}
		else {
			exif_image_size_info->edit_date_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
		}
		tag_offset = reset_offset;

		// Get EXIF IDF offset
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND EXIF IDF OFFSET\n"));
#endif
			break;
		}
		else if (ercd != 0) {
			break;
		}
		else {
			; // DO NOTHING
		}
		data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

		// get next start offset
		ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
		if (ercd != 0) {
			break;
		}

		// maker note
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_MAKERNOTE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			exif_image_size_info->maker_note_offset = 0;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND MAKERNOTE\n"));
#endif
		}
		else if (ercd != 0) {
			exif_image_size_info->maker_note_offset = 0;
			break;
		}
		else {
			exif_image_size_info->maker_note_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
		}
		tag_offset = reset_offset;

		// image width
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELXDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			exif_image_size_info->x_size_offset = 0;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_X_DIMENSION\n"));
#endif
		}
		else if (ercd != 0) {
			exif_image_size_info->x_size_offset = 0;
			break;
		}
		else {
			exif_image_size_info->x_size_offset = (tag_offset + value_offset) + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}
		tag_offset = reset_offset;

		// image height
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELYDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			exif_image_size_info->y_size_offset = 0;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_WRN_PRINT
			BF_Fs_User_Custom_Wrn_Printer(("CAN NOT FIND PIXEL_Y_DIMENSION\n"));
#endif
		}
		else if (ercd != 0) {
			exif_image_size_info->y_size_offset = 0;
			break;
		}
		else {
			exif_image_size_info->y_size_offset = (tag_offset + value_offset) + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		}

		// get NEXT IFD Pointer
		ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, first_ifd_offset, &tag_offset, &reset_offset, &total_buffer);
		if (ercd != 0) {
			break;
		}

		// search JPEG InterchangeFormat IFD (0x201)
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			exif_image_size_info->thumb_offset = 0;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND JPEG INTER_CHANGE_FORMAT\n"));
#endif
			break;
		}
		else if (ercd != 0) {
			exif_image_size_info->thumb_offset = 0;
			break;
		}
		else {
			; // DO NOTHING
		}
		exif_image_size_info->thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

/*		if (exif_image_size_info->qvga_flag == 1) {
			exif_image_size_info->thumb_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset) + 1;
		}
*/
		// JPEG InterchangeFormatLength tag is 0x202
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			exif_image_size_info->thumb_length_offset = 0;
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND JPEG INTER_CHANGE_FORMAT_LENGTH\n"));
#endif
			break;
		}
		else if (ercd != 0) {
			exif_image_size_info->thumb_length_offset = 0;
			break;
		}
		else {
			; // DO NOTHING
		}
		exif_image_size_info->thumb_length_offset = (tag_offset + value_offset) + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
		break;
	}

	// make field length value
	exif_image_size_info->field_length_value = exif_image_size_info->thumb_offset + exif_image_size_info->thumb_length_value - D_BF_DCF_EXIF_SOI_SIZE;
//	if ((exif_image_size_info->thumb_length_value & 0x1) != 0 ) {	// if size is odd, make even
//		exif_image_size_info->field_length_value++;
//	}

	ercd2 = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)buffer);
	if (ercd2 != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd2));
#endif
		ercd = ercd2;
	}

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	if (g_bf_dcf_exif_big_or_lit == D_BF_DCF_EXIF_LIT_EN) {
		// make pixel x value
		dcf_exif_swap_long(&exif_image_size_info->x_size_value);

		// make pixel y value
		dcf_exif_swap_long(&exif_image_size_info->y_size_value);

		// make thumb_size value
		dcf_exif_swap_long(&exif_image_size_info->thumb_length_value);
	}
#else
	if (g_bf_dcf_exif_big_or_lit == D_BF_DCF_EXIF_BIG_EN) {
		// make pixel x value
		dcf_exif_swap_long(&exif_image_size_info->x_size_value);

		// make pixel y value
		dcf_exif_swap_long(&exif_image_size_info->y_size_value);

		// make thumb_size value
		dcf_exif_swap_long(&exif_image_size_info->thumb_length_value);
	}
#endif

	// reset file pointer
	ercd = BF_Mdf_Wrapper_Lseek(file_id, 0, D_BF_MDF_WRAPPER_SEEK_SET, &result);
	if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
	}

	return ercd;
}

/**
 * @brief search half word code in target buffer.
 * @param i:*add target address
 * @param i:size buffer size(search range)
 * @param i:code search code
 * @param i:start_offset start offset
 * @param o:*value_offset pointer of value offset
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Exif_Search_Half_Word_Code(FW_UCHAR* add, FW_ULONG size, FW_USHORT code, FW_ULONG start_offset, FW_ULONG* value_offset)
{
	if (add == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}
	if (value_offset == NULL) {
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	*value_offset = start_offset;

	while (dcf_exif_get_short_value(add + *value_offset) != code) {
		*value_offset += 2;
		if (*value_offset >= size) {
			break;
		}
	}

	return 0;
}

/**
 * @brief exchange date information to date charactor.
 * @param i:datetype date information
 * @param o:*addr date charactor address
 * @return FW_VOID
 * @note none
 * @attention date charactor length is 20byte.
 */
FW_VOID BF_Dcf_Exif_Exchange_Datetime(T_BF_DCF_IF_DATE datetype, FW_UCHAR* addr)
{
#ifdef CO_BF_FS_USER_CUSTOM_USE_BCD_DATE
	dcf_exif_set_bcd_datetime(addr, datetype);
#else
	dcf_exif_set_bin_datetime(addr, datetype);
#endif
}

/**
 * @brief set exif data
 * @param i:ifd  IFD index
 * @param i: tag  EXIF TAG ID  
 * @param i: count  value length (byte)
 * @param o: addr  EXIF value addr
 * @return  success:0 false:-1
 * @note none
 */
FW_INT32 BF_Dcf_Exif_Set_Value(FW_USHORT ifd, FW_USHORT tag, FW_USHORT type, FW_USHORT count, FW_UCHAR* addr)
{
	switch (type) {
		case 0xFFFF:
		case D_BF_DCF_EXIF_BYTE_T: // FALL THROUGH
		case D_BF_DCF_EXIF_ASCII_T: // FALL THROUGH
		case D_BF_DCF_EXIF_UNDEFINED_T:
			break;

		case D_BF_DCF_EXIF_SHORT_T:
			count *= D_BF_DCF_EXIF_SHORT_S;
			break;

		case D_BF_DCF_EXIF_LONG_T:
			count *= D_BF_DCF_EXIF_LONG_S;
			break;

		case D_BF_DCF_EXIF_RATIONAL_T:
		case D_BF_DCF_EXIF_SRATIONAL_T:
			count *= D_BF_DCF_EXIF_RATIONAL_S;
			break;

		default:
			return -1;
	}

	// update EXIF header
	if (ifd == FJ_IFD_0) {
		switch (tag) {
			case FJ_IFD0_TAG_IMGDESC_:	// Title
				if (count > FJ_CUSTOM_EXIF_IMG_DESCRIPTION_S) {
					return -1;
				}
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.ImageDescription.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.Ifd0_Num -= 1;
					dcf_exif_delete_ifd0_tag(FJ_CUSTOM_EXIF_IMG_DESCRIPTION_S, ifd, FJ_IFD0_TAG_IMGDESC_);
					break;
				}
				// Exif Charctors tag area clear and count set]
				memset((FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueImageDescription, 0, FJ_CUSTOM_EXIF_IMG_DESCRIPTION_S);
				g_bf_dcf_exif_jpeg_header_first.Ifd0.ImageDescription.Count = count;
				memcpy((FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueImageDescription, addr, count);
				break;

			case FJ_IFD0_TAG_MAKE_:	// Make
				if (count > FJ_CUSTOM_EXIF_MAKE_S) {
					return -1;
				}
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Make.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.Ifd0_Num -= 1;
					dcf_exif_delete_ifd0_tag(FJ_CUSTOM_EXIF_MAKE_S, ifd, FJ_IFD0_TAG_MAKE_);
					break;
				}
				// Exif Charctors tag area clear and count set]
				memset((FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueMake, 0, FJ_CUSTOM_EXIF_MAKE_S);
				g_bf_dcf_exif_jpeg_header_first.Ifd0.Make.Count = count;
				memcpy((FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueMake, addr, count);
				break;

			case FJ_IFD0_TAG_MODEL_:	// Model
				if (count > FJ_CUSTOM_EXIF_MODEL_S) {
					return -1;
				}
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Model.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.Ifd0_Num -= 1;
					dcf_exif_delete_ifd0_tag(FJ_CUSTOM_EXIF_MODEL_S, ifd, FJ_IFD0_TAG_MODEL_);
					break;
				}
				// Exif Charctors tag area clear and count set]
				memset((FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueModel, 0, FJ_CUSTOM_EXIF_MODEL_S);
				g_bf_dcf_exif_jpeg_header_first.Ifd0.Model.Count = count;
				memcpy((FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueModel, addr, count);
				break;

			case FJ_IFD0_TAG_ORIENT_:	// Orientation
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.Ifd0_Num -= 1;
					dcf_exif_delete_ifd0_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_IFD0_TAG_XRES_:	// XResolution
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueXResolution, addr, count);
				break;

			case FJ_IFD0_TAG_YRES_:	// YResolution
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueYResolution, addr, count);
				break;

			case FJ_IFD0_TAG_RESUNIT_:	// ResolutionUnit
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.ResolutionUnit.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_IFD0_TAG_SOFT_:	// Software
				if (count > FJ_CUSTOM_EXIF_SOFT_S) {
					return -1;
				}
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Software.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.Ifd0_Num -= 1;
					dcf_exif_delete_ifd0_tag(FJ_CUSTOM_EXIF_SOFT_S, ifd, FJ_IFD0_TAG_SOFT_);
					break;
				}
				// Exif Charctors tag area clear and count set]
				memset((FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueSoftware, 0, FJ_CUSTOM_EXIF_SOFT_S);
				g_bf_dcf_exif_jpeg_header_first.Ifd0.Software.Count = count;
				memcpy((FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueSoftware, addr, count);
				break;

			case FJ_IFD0_TAG_DATETIME_:	// DateTime
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.Ifd0_Num -= 1;
					dcf_exif_delete_ifd0_tag(20, ifd, FJ_IFD0_TAG_DATETIME_);
					break;
				}
				break;

			case FJ_IFD0_TAG_YCBCRPOS_:	//YCbCrPositioning
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.YCbCrPositioning.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_IFD0_TAG_COPYRIGHT_:	// Copyright
				if (count > FJ_CUSTOM_EXIF_COPYRIGHT_S) {
					return -1;
				}
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Copyright.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.Ifd0_Num -= 1;
					dcf_exif_delete_ifd0_tag(FJ_CUSTOM_EXIF_COPYRIGHT_S, ifd, FJ_IFD0_TAG_COPYRIGHT_);
					break;
				}
				// Exif Charctors tag area clear and count set]
				memset((FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueCopyright, 0, FJ_CUSTOM_EXIF_COPYRIGHT_S);
				g_bf_dcf_exif_jpeg_header_first.Ifd0.Copyright.Count = count;
				memcpy((FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueCopyright, addr, count);
				break;

			case FJ_IFD0_TAG_EXIFPTR_:	// ExifIFDPointer
//				memcpy(&g_bf_dcf_exif_jpeg_header_first.Ifd0.ExifIFDPointer.ValueOffset.LValue, addr, count);
				break;

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
			case FJ_IFD0_TAG_PRINTIM_:	// PrintIM
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.PrintIM.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.Ifd0_Num -= 1;
					dcf_exif_delete_ifd0_tag(FJ_CUSTOM_EXIF_PRINTIM_S, ifd, FJ_IFD0_TAG_PRINTIM_);
					break;
				}
				memset((FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValuePrintIM, 0, FJ_CUSTOM_EXIF_PRINTIM_S);
				g_bf_dcf_exif_jpeg_header_first.Ifd0.PrintIM.Count = count;
				memcpy((FW_VOID*)g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValuePrintIM, addr, count);
				break;
#endif
			default:
				return -1;
		}
	}
	else if (ifd == FJ_IFD_EXIF) {
		switch (tag) {
			case FJ_EXIF_TAG_EXPTIME_:	// ExposureTime
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureTime.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(D_BF_DCF_EXIF_RATIONAL_S, ifd, FJ_EXIF_TAG_EXPTIME_);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureTime, addr, count);
				break;

			case FJ_EXIF_TAG_FNUM_:	// FNumber
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.FNumber.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(D_BF_DCF_EXIF_RATIONAL_S, ifd, FJ_EXIF_TAG_FNUM_);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueFNumber, addr, count);
				break;

			case FJ_EXIF_TAG_EXPPROG_:	// ExposureProgram
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureProgram.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureProgram.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_EXIF_TAG_ISOSPEEDRATE_:	// ISOSpeedRatings
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ISOSpeedRatings.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ISOSpeedRatings.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_EXIF_TAG_EXIFVER_:	//ExifVersion
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExifVersion.ValueOffset.LValue, addr, count);
				break;

			case FJ_EXIF_TAG_DATETIMEORIG_:	// DateTimeOriginal
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(20, ifd, FJ_EXIF_TAG_DATETIMEORIG_);
					break;
				}
//				memcpy(g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeOriginal, addr, count);
				break;

			case FJ_EXIF_TAG_DATETIMEDITI_:	// DateTimeDigitized
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(20, ifd, FJ_EXIF_TAG_DATETIMEDITI_);
					break;
				}
//				memcpy(g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeDigitized, addr, count);
				break;

			case FJ_EXIF_TAG_COMPCONFIGURE_:	// ComponentsConfiguration
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ComponentsConfiguration.ValueOffset.LValue, addr, count);
				break;

			case FJ_EXIF_TAG_COMPRESSBPP_:	// CompressedBitsPerPixel
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.CompressedBitsPerPixel.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(D_BF_DCF_EXIF_RATIONAL_S, ifd, FJ_EXIF_TAG_COMPRESSBPP_);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueCompressedBitsPerPixel, addr, count);
				break;

			case FJ_EXIF_TAG_SHUTTER_:	// ShutterSpeedValue
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ShutterSpeedValue.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(D_BF_DCF_EXIF_SRATIONAL_S, ifd, FJ_EXIF_TAG_SHUTTER_);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueShutterSpeedValue, addr, count);
				break;

			case FJ_EXIF_TAG_APERTURE_:	// ApertureValue
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ApertureValue.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(D_BF_DCF_EXIF_RATIONAL_S, ifd, FJ_EXIF_TAG_APERTURE_);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueApertureValue, addr, count);
				break;

			case FJ_EXIF_TAG_EXPBIAS_:	// ExposureBiasValue
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureBiasValue.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(D_BF_DCF_EXIF_SRATIONAL_S, ifd, FJ_EXIF_TAG_EXPBIAS_);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureBiasValue, addr, count);
				break;

			case FJ_EXIF_TAG_MAXAPTURE_:	// MaxApertureValue
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.MaxApertureValue.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(D_BF_DCF_EXIF_RATIONAL_S, ifd, FJ_EXIF_TAG_MAXAPTURE_);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueMaxApertureValue, addr, count);
				break;

			case FJ_EXIF_TAG_METERMODE_:	// MeteringMode
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.MeteringMode.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.MeteringMode.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_EXIF_TAG_LIGHTSOURCE_:	// LightSource
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.LightSource.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.LightSource.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_EXIF_TAG_FLASH_:	// Flash
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Flash.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Flash.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_EXIF_TAG_FOCALLEN_:	// FocalLength
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(D_BF_DCF_EXIF_RATIONAL_S, ifd, FJ_EXIF_TAG_FOCALLEN_);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueFocalLength, addr, count);
				break;

			case FJ_EXIF_TAG_MAKERNOTE_:	// MakerNote
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Count, ifd, FJ_EXIF_TAG_MAKERNOTE_);
					break;
				}
				break;

			case FJ_EXIF_TAG_FLRESHPIXVER_:	// FlashPixVersion
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.FlashPixVersion.ValueOffset.LValue, addr, count);
				break;

			case FJ_EXIF_TAG_COLORSPACE_:	// ColorSpace
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ColorSpace.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_EXIF_TAG_PEXELXDIM_:	// PixelXDimension
//				memcpy(&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.SValue.Value, addr, count);
				if (type == D_BF_DCF_EXIF_SHORT_T) {
					memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.SValue.Value, addr, count);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.SValue.dummy = 0;
					g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.Type = D_BF_DCF_EXIF_SHORT_T;
				}
				else {
					memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.ValueOffset.LValue, addr, count);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.Type = D_BF_DCF_EXIF_LONG_T;
				}
				break;

			case FJ_EXIF_TAG_PEXELYDIM_:	// PixelYDimension
//				memcpy(&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.SValue.Value, addr, count);
				if (type == D_BF_DCF_EXIF_SHORT_T) {
					memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.SValue.Value, addr, count);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.SValue.dummy = 0;
					g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.Type = D_BF_DCF_EXIF_SHORT_T;
				}
				else {
					memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.ValueOffset.LValue, addr, count);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.Type = D_BF_DCF_EXIF_LONG_T;
				}
				break;

			case FJ_EXIF_TAG_ITOPIFDPTR_:    // InteroperabilityIFDPointer
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
//				memcpy(&g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer.ValueOffset.LValue, addr, count);
				break;

			case FJ_EXIF_TAG_SENSOR_:	// SensingMethod
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.SensingMethod.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.SensingMethod.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_EXIF_TAG_FILESOURCE_:	// FileSource
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.FileSource.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.FileSource.ValueOffset.LValue, addr, count);
				break;

			case FJ_EXIF_TAG_SENCETYPE_:	// SceneType
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.SceneType.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.SceneType.ValueOffset.LValue, addr, count);
				break;

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
			case FJ_EXIF_TAG_CUSTOMRENDERED_:	// CustomRendered
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.CustomRendered.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.CustomRendered.ValueOffset.SValue.Value, addr, count);
				break;
#endif

			case FJ_EXIF_TAG_EXPOSUREMODE_:	// ExposureMode
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureMode.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureMode.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_EXIF_TAG_WB_:	// WhiteBalanceMode
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.WhiteBalanceMode.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.WhiteBalanceMode.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_EXIF_TAG_DZOOM_:	// DigitalZommRatio
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.DigitalZoomRatio.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(D_BF_DCF_EXIF_RATIONAL_S, ifd, FJ_EXIF_TAG_DZOOM_);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDigitalZoomRatio, addr, count);
				break;

			case FJ_EXIF_TAG_FOCALLENGTH35mm_:	// FocalLength35mm
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength35mmFilm.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength35mmFilm.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_EXIF_TAG_CAPTURETYPE_:	// CaptureType
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.CaptureType.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.CaptureType.ValueOffset.SValue.Value, addr, count);
				break;

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
			case FJ_EXIF_TAG_CONTRAST_:	// Contrast
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Contrast.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Contrast.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_EXIF_TAG_SATURATION_:	// Saturation
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Saturation.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Saturation.ValueOffset.SValue.Value, addr, count);
				break;
#endif

			case FJ_EXIF_TAG_SHARPNESS_:	// Sharpness
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Sharpness.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num -= 1;
					dcf_exif_delete_exif_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Sharpness.ValueOffset.SValue.Value, addr, count);
				break;

			default:
				return -1;
		}
	}
	else if (ifd == FJ_IFD_INTEROPERABILITY) {
		switch (tag) {
			case FJ_ITOP_TAG_ITOPINDEX_:	// InteroperabilityIndex
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.R98Ifd.InteroperabilityIndex.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_second.R98Ifd_Num -= 1;
					dcf_exif_delete_r98_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.R98Ifd.InteroperabilityIndex.ValueOffset.LValue, addr, count);
				break;

			case FJ_ITOP_TAG_ITOPVERSION_:	// InteroperabilityVersion
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.R98Ifd.InteroperabilityVersion.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_second.R98Ifd_Num -= 1;
					dcf_exif_delete_r98_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.R98Ifd.InteroperabilityVersion.ValueOffset.LValue, addr, count);
				break;

			default:
				return -1;
		}
	}
	else if (ifd == FJ_IFD_1) {
		switch (tag) {
			case FJ_IFD1_TAG_COMPRESS_:	// Compression
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.Compression.ValueOffset.SValue.Value, addr, count);
				break;

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
			case FJ_IFD1_TAG_MAKER1_:	// Maker1
				if (count > FJ_CUSTOM_EXIF_MAKER1_S) {
					return -1;
				}
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.Maker1.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_second.Ifd1_Num -= 1;
					dcf_exif_delete_ifd1_tag(FJ_CUSTOM_EXIF_MAKER1_S, ifd, FJ_IFD1_TAG_MAKER1_);
					break;
				}
				memset((FW_VOID*)g_bf_dcf_exif_jpeg_header_second.Ifd1_value.ValueMaker1, 0, FJ_CUSTOM_EXIF_MAKER1_S);
				g_bf_dcf_exif_jpeg_header_second.Ifd1.Maker1.Count = count;
				memcpy((FW_VOID*)g_bf_dcf_exif_jpeg_header_second.Ifd1_value.ValueMaker1, addr, count);
				break;

			case FJ_IFD1_TAG_MODEL1_:	// Model1
				if (count > FJ_CUSTOM_EXIF_MODEL1_S) {
					return -1;
				}
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.Model1.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_second.Ifd1_Num -= 1;
					dcf_exif_delete_ifd1_tag(FJ_CUSTOM_EXIF_MODEL1_S, ifd, FJ_IFD1_TAG_MODEL1_);
					break;
				}
				memset((FW_VOID*)g_bf_dcf_exif_jpeg_header_second.Ifd1_value.ValueModel1, 0, FJ_CUSTOM_EXIF_MODEL1_S);
				g_bf_dcf_exif_jpeg_header_second.Ifd1.Model1.Count = count;
				memcpy((FW_VOID*)g_bf_dcf_exif_jpeg_header_second.Ifd1_value.ValueModel1, addr, count);
				break;
#endif

			case FJ_IFD1_TAG_ORIENT_:	// Orientation
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.Orientation.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_second.Ifd1_Num -= 1;
					dcf_exif_delete_ifd1_tag(0, 0, 0);
					break;
				}
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.Orientation.ValueOffset.SValue.Value, addr, count);
				break;

			case FJ_IFD1_TAG_XRES_:	// XResolution
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1_value.ValueXResolution, addr, count);
				break;

			case FJ_IFD1_TAG_YRES_:	// YResolution
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1_value.ValueYResolution, addr, count);
				break;

			case FJ_IFD1_TAG_RESUINT_:	// ResolutionUnit
				memcpy((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.ResolutionUnit.ValueOffset.SValue.Value, addr, count);
				break;

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
			case FJ_IFD1_TAG_DATETIME1_:	// DateTime1
				if (type == 0xFFFF) {
					memset((FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.DateTime1.Tag, 0, 2);
					g_bf_dcf_exif_jpeg_header_second.Ifd1_Num -= 1;
					dcf_exif_delete_ifd1_tag(20, ifd, FJ_IFD1_TAG_DATETIME1_);
					break;
				}
				break;
#endif

			case FJ_IFD1_TAG_JPG_INTERCHGFMT_:	// JPEGInterchangeFormat
//				memcpy(&g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormat.ValueOffset.LValue, addr, count);
				break;

			case FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_:	//JPEGInterchangeFormatLength
//				memcpy(&g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormatLength.ValueOffset.LValue, addr, count);
				break;

			default:
				return -1;
		}
	}
	else {
		return -1;
	}

	return 0;
}

#if 0
/**
 * @brief copy exif jpeg header.
 * @param i:*jpg_head source address(jpeg header data address)
 * @param o:*tmp_buff destination address(write data address)
 * @param o:*jpg_head_length pointer of jpeg header length
 * @return error code
 * @note none
 * @attention none
 */
FW_VOID BF_Dcf_Exif_Copy_Exif_Jpeg(FW_CHAR* tmp_buff, FW_ULONG* jpg_head_length)
{
	FW_ULONG length = 0;

	if(tmp_buff == NULL){
		return;
	}
	if(jpg_head_length == NULL){
		return;
	}


	memcpy(tmp_buff, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Soi, D_BF_DCF_EXIF_SHORT_S);
	length += D_BF_DCF_EXIF_SHORT_S;
	memcpy(tmp_buff + length, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.App1, D_BF_DCF_EXIF_APP1_MARKER_S);
	length += D_BF_DCF_EXIF_APP1_MARKER_S;
	memcpy(tmp_buff + length, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Tiff_Head, D_BF_DCF_EXIF_TIFF_HEADER_S);
	length += D_BF_DCF_EXIF_TIFF_HEADER_S;
	memcpy(tmp_buff + length, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_Num, D_BF_DCF_EXIF_SHORT_S);
	length += D_BF_DCF_EXIF_SHORT_S;
	memcpy(tmp_buff + length, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Ifd0, D_BF_DCF_EXIF_JPEG_ZERO_IFD_S);
	length += D_BF_DCF_EXIF_JPEG_ZERO_IFD_S;
	memcpy(tmp_buff + length, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num, D_BF_DCF_EXIF_SHORT_S);
	length += D_BF_DCF_EXIF_SHORT_S;
	memcpy(tmp_buff + length, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd, D_BF_DCF_EXIF_JPEG_EXIF_IFD_S);
	length += D_BF_DCF_EXIF_JPEG_EXIF_IFD_S;
	//memcpy(tmp_buff+length, gmaker_note, g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Count);
	if( 1 != FJ_Host_CreateMakerNoteCB(tmp_buff + length, g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Count, length, NULL, 0) ){
		// Fail
		memset(tmp_buff + length, 0, g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Count);
	}
	length += g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Count;
	memcpy(tmp_buff + length, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.R98Ifd_Num, D_BF_DCF_EXIF_SHORT_S);
	length += D_BF_DCF_EXIF_SHORT_S;
	memcpy(tmp_buff + length, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.R98Ifd, D_BF_DCF_EXIF_JPEG_R98_IFD_S);
	length += D_BF_DCF_EXIF_JPEG_R98_IFD_S;
	memcpy(tmp_buff + length, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1_Num, D_BF_DCF_EXIF_SHORT_S);
	length += D_BF_DCF_EXIF_SHORT_S;
	memcpy(tmp_buff + length, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_second.Ifd1, D_BF_DCF_EXIF_JPEG_FIRST_IFD_S);
	*jpg_head_length = length + D_BF_DCF_EXIF_JPEG_FIRST_IFD_S;
}

#else

FW_VOID BF_Dcf_Exif_Copy_Exif_Jpeg(FW_CHAR* tmp_buff, FW_ULONG* jpg_head_length)
{
	FW_ULONG length = 0;
	FW_ULONG tmp_offset = 0;
	FW_INT32 ercd;
	FW_ULONG exif_0201_offset = 0; // JPEGInterchangeFormat

	if (tmp_buff == NULL) {
		return;
	}
	if (jpg_head_length == NULL) {
		return;
	}

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
#else
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
#endif
#else
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
#endif
#endif

	// Header Set Start
	memcpy(tmp_buff, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Soi, D_BF_DCF_EXIF_SHORT_S);
	length += D_BF_DCF_EXIF_SHORT_S;
	memcpy(tmp_buff + length, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.App1, D_BF_DCF_EXIF_APP1_MARKER_S);
	length += D_BF_DCF_EXIF_APP1_MARKER_S;
//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Tiff_Head, D_BF_DCF_EXIF_TIFF_HEADER_S);
	ercd = dcf_exif_set_exif_tiff_header_to_buf((T_BF_DCF_EXIF_TIFF_HEADER*)&g_bf_dcf_exif_jpeg_header_first.Tiff_Head, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write tiff header err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_TIFF_HEADER_S;

	// IFD0 Tag Set Start
//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_Num, D_BF_DCF_EXIF_SHORT_S);
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_Num, D_BF_DCF_EXIF_SHORT_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 number err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_SHORT_S;

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.ImageDescription.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.ImageDescription, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.ImageDescription, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 ImageDescription err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Make.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.Make, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Make, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 make err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Model.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.Model, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Model, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 Model err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 Orientation err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.XResolution, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.XResolution, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 XResolution err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.YResolution, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.YResolution, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 YResolution err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.ResolutionUnit, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.ResolutionUnit, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 ResolutionUnit err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Software.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.Software, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Software, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 Software err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 DateTime err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.YCbCrPositioning, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.YCbCrPositioning, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 YCbCrPositioning err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Copyright.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.Copyright, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Copyright, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 Copyright err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.ExifIFDPointer, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.ExifIFDPointer, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 ExifIFDPointer err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

#ifdef CO_BF_DCF_IF_SET_GPSTAG
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.GpsIFDPointer, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 ExifIFDPointer err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;
#endif

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.PrintIM.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.PrintIM, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.PrintIM, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 PrintIM err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}
#endif
	tmp_offset = length;
	length += sizeof(FW_ULONG);

	// IFD0 Value Set Start
	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.ImageDescription.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueImageDescription, D_BF_DCF_EXIF_V_DESCRIPTION_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueImageDescription, 
											g_bf_dcf_exif_jpeg_header_first.Ifd0.ImageDescription.Type, 
											D_BF_DCF_EXIF_V_DESCRIPTION_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValueImageDescription err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_DESCRIPTION_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Make.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueMake, D_BF_DCF_EXIF_V_MAKE_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueMake, 
											g_bf_dcf_exif_jpeg_header_first.Ifd0.Make.Type, 
											D_BF_DCF_EXIF_V_MAKE_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValueMake err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_MAKE_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Model.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueModel, D_BF_DCF_EXIF_V_MODEL_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueModel, 
											g_bf_dcf_exif_jpeg_header_first.Ifd0.Model.Type, 
											D_BF_DCF_EXIF_V_MODEL_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValueModel err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_MODEL_S;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueXResolution, D_BF_DCF_EXIF_RATIONAL_S);
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueXResolution, 
										g_bf_dcf_exif_jpeg_header_first.Ifd0.XResolution.Type, 
										g_bf_dcf_exif_jpeg_header_first.Ifd0.XResolution.Count, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValueXResolution err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_RATIONAL_S;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueYResolution, D_BF_DCF_EXIF_RATIONAL_S);
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueYResolution, 
										g_bf_dcf_exif_jpeg_header_first.Ifd0.YResolution.Type, 
										g_bf_dcf_exif_jpeg_header_first.Ifd0.YResolution.Count, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValueYResolution err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_RATIONAL_S;

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Software.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueSoftware, D_BF_DCF_EXIF_V_SOFT_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueSoftware, 
											g_bf_dcf_exif_jpeg_header_first.Ifd0.Software.Type, 
											D_BF_DCF_EXIF_V_SOFT_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValueSoftware err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_SOFT_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueDateTime, D_BF_DCF_EXIF_V_DATETIME_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueDateTime, 
											g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime.Type, 
											D_BF_DCF_EXIF_V_DATETIME_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValueDateTime err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_DATETIME_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Copyright.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueCopyright, D_BF_DCF_EXIF_V_COPYRIGHT_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueCopyright, 
											g_bf_dcf_exif_jpeg_header_first.Ifd0.Copyright.Type, 
											D_BF_DCF_EXIF_V_COPYRIGHT_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValueCopyright err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_COPYRIGHT_S;
	}

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.PrintIM.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValuePrintIM, D_BF_DCF_EXIF_V_PRINTIM_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValuePrintIM, 
											g_bf_dcf_exif_jpeg_header_first.Ifd0.PrintIM.Type, 
											D_BF_DCF_EXIF_V_PRINTIM_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValuePrintIM err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_PRINTIM_S;
	}
#endif

	// EXIF Tag Set Start
//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num, D_BF_DCF_EXIF_SHORT_S);
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num, D_BF_DCF_EXIF_SHORT_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write ExifIFD number err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_SHORT_S;

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureTime.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureTime, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureTime, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD ExposureTime err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.FNumber.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.FNumber, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.FNumber, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD FNumber err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureProgram.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureProgram, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureProgram, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD ExposureProgram err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.ISOSpeedRatings.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.ISOSpeedRatings, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ISOSpeedRatings, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD ISOSpeedRatings err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExifVersion, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExifVersion, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD ISOSpeedRatings err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD DateTimeOriginal err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD DateTimeDigitized err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.ComponentsConfiguration, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ComponentsConfiguration, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD ComponentsConfiguration err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.CompressedBitsPerPixel.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.CompressedBitsPerPixel, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.CompressedBitsPerPixel, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD CompressedBitsPerPixel err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.ShutterSpeedValue.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.ShutterSpeedValue, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ShutterSpeedValue, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD ShutterSpeedValue err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.ApertureValue.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.ApertureValue, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ApertureValue, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD ApertureValue err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureBiasValue.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureBiasValue, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureBiasValue, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD ExposureBiasValue err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.MaxApertureValue.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.MaxApertureValue, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.MaxApertureValue, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD MaxApertureValue err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.MeteringMode.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.MeteringMode, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.MeteringMode, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD MeteringMode err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.LightSource.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.LightSource, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.LightSource, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD LightSource err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}
	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.Flash.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.Flash, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Flash, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD Flash err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD FocalLength err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD MakerNote err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.FlashPixVersion, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.FlashPixVersion, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD FlashPixVersion err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.ColorSpace, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ColorSpace, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD ColorSpace err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD PixelXDimension err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD PixelYDimension err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD InteroperabilityIFDPointer err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.SensingMethod.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.SensingMethod, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.SensingMethod, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD SensingMethod err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.FileSource.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.FileSource, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.FileSource, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD FileSource err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.SceneType.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.SceneType, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.SceneType, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD SceneType err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.CustomRendered.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.CustomRendered, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.CustomRendered, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD CustomRendered err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}
#endif
	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureMode.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureMode, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureMode, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD ExposureMode err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.WhiteBalanceMode.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.WhiteBalanceMode, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.WhiteBalanceMode, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD WhiteBalanceMode err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.DigitalZoomRatio.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.DigitalZoomRatio, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.DigitalZoomRatio, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD DigitalZoomRatio err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength35mmFilm.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength35mmFilm, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength35mmFilm, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD FocalLength35mmFilm err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.CaptureType.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.CaptureType, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.CaptureType, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD CaptureType err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.Contrast.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.Contrast, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Contrast, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD Contrast err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.Saturation.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.Saturation, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Saturation, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD Saturation err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}
#endif

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.Sharpness.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.Sharpness, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.Sharpness, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD Sharpness err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.NextIFDOffset, sizeof(FW_ULONG));
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.NextIFDOffset, D_BF_DCF_EXIF_LONG_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD NextIFDOffset err: ercd=%d\n", ercd));
#endif
	}
	length += sizeof(FW_ULONG);

	// Exif Value Set Start
	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureTime.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureTime, D_BF_DCF_EXIF_RATIONAL_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureTime, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureTime.Type, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureTime.Count, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD ValueExposureTime err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_RATIONAL_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.FNumber.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueFNumber, D_BF_DCF_EXIF_RATIONAL_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueFNumber, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.FNumber.Type, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.FNumber.Count, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD ValueFNumber err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_RATIONAL_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeOriginal, D_BF_DCF_EXIF_V_DATEORIG_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeOriginal, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal.Type, 
											D_BF_DCF_EXIF_V_DATEORIG_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD ValueDateTimeOriginal err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_DATEORIG_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeDigitized, D_BF_DCF_EXIF_V_DATEORIG_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeDigitized, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized.Type, 
											D_BF_DCF_EXIF_V_DATEORIG_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD ValueDateTimeDigitized err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_DATEORIG_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.CompressedBitsPerPixel.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueCompressedBitsPerPixel, D_BF_DCF_EXIF_RATIONAL_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueCompressedBitsPerPixel, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.CompressedBitsPerPixel.Type, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.CompressedBitsPerPixel.Count, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD ValueCompressedBitsPerPixel err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_RATIONAL_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.ShutterSpeedValue.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueShutterSpeedValue, D_BF_DCF_EXIF_SRATIONAL_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueShutterSpeedValue, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.ShutterSpeedValue.Type, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.ShutterSpeedValue.Count, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD ValueShutterSpeedValue err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_SRATIONAL_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.ApertureValue.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueApertureValue, D_BF_DCF_EXIF_RATIONAL_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueApertureValue, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.ApertureValue.Type, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.ApertureValue.Count, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD ValueApertureValue err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_RATIONAL_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureBiasValue.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureBiasValue, D_BF_DCF_EXIF_SRATIONAL_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueExposureBiasValue, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureBiasValue.Type, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureBiasValue.Count, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD ValueExposureBiasValue err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_SRATIONAL_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.MaxApertureValue.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueMaxApertureValue, D_BF_DCF_EXIF_RATIONAL_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueMaxApertureValue, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.MaxApertureValue.Type, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.MaxApertureValue.Count, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD ValueMaxApertureValue err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_RATIONAL_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueFocalLength, D_BF_DCF_EXIF_RATIONAL_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueFocalLength, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength.Type, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength.Count, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD ValueFocalLength err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_RATIONAL_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.DigitalZoomRatio.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDigitalZoomRatio, D_BF_DCF_EXIF_RATIONAL_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDigitalZoomRatio, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.DigitalZoomRatio.Type, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.DigitalZoomRatio.Count, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD ValueDigitalZoomRatio err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_RATIONAL_S;
	}

	// MakerNote Set Start
	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Tag != 0) {
		if ( 1 != FJ_Host_CreateMakerNoteCB((CHAR*)tmp_buff + length, g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Count, length, NULL, 0)) {
			// Fail
			memset(tmp_buff + length, 0, g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Count);
		}
		length += g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.Count;
	}

	// R98IFD Set Start
//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.R98Ifd_Num, D_BF_DCF_EXIF_SHORT_S);
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.R98Ifd_Num, D_BF_DCF_EXIF_SHORT_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val R98 IFD number err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_SHORT_S;

	if (g_bf_dcf_exif_jpeg_header_second.R98Ifd.InteroperabilityIndex.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.R98Ifd.InteroperabilityIndex, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.R98Ifd.InteroperabilityIndex, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write R98 IFD InteroperabilityIndex err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_second.R98Ifd.InteroperabilityVersion.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.R98Ifd.InteroperabilityVersion, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.R98Ifd.InteroperabilityVersion, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write R98 IFD InteroperabilityVersion err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.R98Ifd.NextIFDOffset, sizeof(FW_ULONG) );
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.R98Ifd.NextIFDOffset, D_BF_DCF_EXIF_LONG_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val R98 IFD NextIFDOffset err: ercd=%d\n", ercd));
#endif
	}
	length += sizeof(FW_ULONG);

#ifdef CO_BF_DCF_IF_SET_GPSTAG
	// GPS Tag Set Start
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.Gps_Num, D_BF_DCF_EXIF_SHORT_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val GPS number err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_SHORT_S;

	if (g_bf_dcf_exif_jpeg_header_second.Gps.GPSLatitudeRef.Tag != 0) {
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Gps.GPSLatitudeRef, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write GPS LatitudeRef err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_second.Gps.GPSLatitude.Tag != 0) {
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Gps.GPSLatitude, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write GPS Latitude err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_second.Gps.GPSLongitudeRef.Tag != 0) {
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Gps.GPSLongitudeRef, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write GPS LatitudeRef err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_second.Gps.GPSLongitude.Tag != 0) {
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Gps.GPSLongitude, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write GPS Latitude err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_second.Gps.GPSImgDirectionRef.Tag != 0) {
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Gps.GPSImgDirectionRef, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write GPS LatitudeRef err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_second.Gps.GPSImgDirection.Tag != 0) {
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Gps.GPSImgDirection, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write GPS Latitude err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}
	
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.Gps.NextIFDOffset, D_BF_DCF_EXIF_LONG_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val GPS NextIFDOffset err: ercd=%d\n", ercd));
#endif
	}
	length += sizeof(FW_ULONG);

	// GPS Value Set Start
	if (g_bf_dcf_exif_jpeg_header_second.Gps.GPSLatitude.Tag != 0) {
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLatitude, 
											g_bf_dcf_exif_jpeg_header_second.Gps.GPSLatitude.Type, 
											3, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val GPS Latitude err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_RATIONAL_S*3;
	}

	if (g_bf_dcf_exif_jpeg_header_second.Gps.GPSLongitude.Tag != 0) {
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSLongitude, 
											g_bf_dcf_exif_jpeg_header_second.Gps.GPSLongitude.Type, 
											3, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val GPS Latitude err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_RATIONAL_S*3;
	}

	if (g_bf_dcf_exif_jpeg_header_second.Gps.GPSImgDirection.Tag != 0) {
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.Gps_value.ValueGPSImgDirection, 
											g_bf_dcf_exif_jpeg_header_second.Gps.GPSImgDirection.Type, 
											1, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val GPS Latitude err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_RATIONAL_S;
	}
#endif

	// IFD1 Tag Set Start
//	memcpy(tmp_buff + tmp_offset, &g_bf_dcf_exif_jpeg_header_first.Ifd0.offset_to_1stIFD, sizeof(FW_ULONG));
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.offset_to_1stIFD, D_BF_DCF_EXIF_LONG_T, 1, tmp_buff + tmp_offset);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 offset_to_1stIFD err: ercd=%d\n", ercd));
#endif
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.Ifd1_Num, D_BF_DCF_EXIF_SHORT_S);
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.Ifd1_Num, D_BF_DCF_EXIF_SHORT_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD1 number err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_SHORT_S;


	if (g_bf_dcf_exif_jpeg_header_second.Ifd1.Compression.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.Ifd1.Compression, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.Compression, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD1 Compression err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	if (g_bf_dcf_exif_jpeg_header_second.Ifd1.Maker1.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.Ifd1.Maker1, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.Maker1, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD1 Maker1 err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_second.Ifd1.Model1.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.Ifd1.Model1, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.Model1, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD1 Model1 err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}
#endif

	if (g_bf_dcf_exif_jpeg_header_second.Ifd1.Orientation.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.Ifd1.Orientation, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.Orientation, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
	#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD1 Orientation err: ercd=%d\n", ercd));
	#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.Ifd1.XResolution, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.XResolution, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD1 XResolution err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.Ifd1.YResolution, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.YResolution, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD1 YResolution err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.Ifd1.ResolutionUnit, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.ResolutionUnit, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD1 ResolutionUnit err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	if (g_bf_dcf_exif_jpeg_header_second.Ifd1.DateTime1.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.Ifd1.DateTime1, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.DateTime1, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD1 DateTime1 err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}
#endif


//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormat, D_BF_DCF_EXIF_T_IFD_SIZE);
	exif_0201_offset = length;
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormat, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD1 JPEGInterchangeFormat err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormatLength, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormatLength, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD1 JPEGInterchangeFormatLength err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.Ifd1.NextIFDOffset, sizeof(FW_ULONG));
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.NextIFDOffset, D_BF_DCF_EXIF_LONG_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD1 NextIFDOffset err: ercd=%d\n", ercd));
#endif
	}
	length += sizeof(FW_ULONG);

	// IFD1 Value Set Start
//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.Ifd1_value, D_BF_DCF_EXIF_JPEG_FIRST_VALUE_S);
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	if (g_bf_dcf_exif_jpeg_header_second.Ifd1.Maker1.Tag != 0) {
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.Ifd1_value.ValueMaker1, 
											g_bf_dcf_exif_jpeg_header_second.Ifd1.Maker1.Type, 
											D_BF_DCF_EXIF_V_MAKER1_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD1 ValueMaker1 err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_MAKER1_S;
	}

	if (g_bf_dcf_exif_jpeg_header_second.Ifd1.Model1.Tag != 0) {
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.Ifd1_value.ValueModel1, 
											g_bf_dcf_exif_jpeg_header_second.Ifd1.Model1.Type, 
											D_BF_DCF_EXIF_V_MODEL1_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD1 ValueModel1 err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_MODEL1_S;
	}
#endif

	if (g_bf_dcf_exif_jpeg_header_second.Ifd1.XResolution.Tag != 0) {
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.Ifd1_value.ValueXResolution, 
											g_bf_dcf_exif_jpeg_header_second.Ifd1.XResolution.Type, 
											g_bf_dcf_exif_jpeg_header_second.Ifd1.XResolution.Count, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD1 ValueXResolution err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_RATIONAL_S;
	}

	if (g_bf_dcf_exif_jpeg_header_second.Ifd1.YResolution.Tag != 0) {
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.Ifd1_value.ValueYResolution, 
											g_bf_dcf_exif_jpeg_header_second.Ifd1.YResolution.Type, 
											g_bf_dcf_exif_jpeg_header_second.Ifd1.YResolution.Count, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD1 ValueYResolution err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_RATIONAL_S;
	}

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	if (g_bf_dcf_exif_jpeg_header_second.Ifd1.DateTime1.Tag != 0) {
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.Ifd1_value.ValueDateTime1, 
											g_bf_dcf_exif_jpeg_header_second.Ifd1.DateTime1.Type, 
											g_bf_dcf_exif_jpeg_header_second.Ifd1.DateTime1.Count, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD1 ValueDateTime1 err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_DATETIME1_S;
	}
#endif

#if 1 // padding 4
	{
		UINT32	padding;
		padding = (length%4);
		if( padding ){
			padding = 4 - padding;
			memset( (VOID*)(tmp_buff + length), 0xFF, padding);
			length += padding;

			BF_Fs_User_Custom_Info_Printer(( "Exif Padding = %d\n", (INT32)padding ));
			g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormat.ValueOffset.LValue += padding;
			dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormat, tmp_buff + exif_0201_offset);
		}
	}
#endif

//	*jpg_head_length = length + D_BF_DCF_EXIF_JPEG_FIRST_VALUE_S;
	*jpg_head_length = length;

	dcf_exif_init_tag_offset();
}

// Initialize Tag Value
FW_VOID dcf_exif_init_tag_offset(FW_VOID)
{
	g_bf_dcf_exif_jpeg_header_first.Ifd0_Num = D_BF_DCF_EXIF_IFD_NUM_OF_IFD0;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num = D_BF_DCF_EXIF_IFD_NUM_OF_IFDEXIF;
	g_bf_dcf_exif_jpeg_header_second.R98Ifd_Num = D_BF_DCF_EXIF_IFD_NUM_OF_R98;
#ifdef CO_BF_DCF_IF_SET_GPSTAG
	g_bf_dcf_exif_jpeg_header_second.Gps_Num = D_BF_DCF_EXIF_IFD_NUM_OF_GPS;
#endif
	g_bf_dcf_exif_jpeg_header_second.Ifd1_Num = D_BF_DCF_EXIF_IFD_NUM_OF_IFD1;

	g_bf_dcf_exif_jpeg_header_first.Ifd0.ImageDescription.ValueOffset.Offset = D_BF_DCF_EXIF_O_DESCRIPTION;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Make.ValueOffset.Offset = D_BF_DCF_EXIF_O_MAKE;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Model.ValueOffset.Offset = D_BF_DCF_EXIF_O_MODEL;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.XResolution.ValueOffset.Offset = D_BF_DCF_EXIF_O_XRESOL;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.YResolution.ValueOffset.Offset = D_BF_DCF_EXIF_O_YRESOL;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Software.ValueOffset.Offset = D_BF_DCF_EXIF_O_SOFT;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime.ValueOffset.Offset = D_BF_DCF_EXIF_O_DATETIME;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Copyright.ValueOffset.Offset = D_BF_DCF_EXIF_O_COPYRIGHT;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.ExifIFDPointer.ValueOffset.LValue = D_BF_DCF_EXIF_O_EXIFPOINTER;
#ifdef CO_BF_DCF_IF_SET_GPSTAG
	g_bf_dcf_exif_jpeg_header_first.Ifd0.GpsIFDPointer.ValueOffset.LValue = D_BF_DCF_EXIF_GPS_IFD_POINTER;
#endif
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	g_bf_dcf_exif_jpeg_header_first.Ifd0.PrintIM.ValueOffset.Offset = D_BF_DCF_EXIF_O_PRINTIM;
#endif

	g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureTime.ValueOffset.Offset = D_BF_DCF_EXIF_O_EXPTIME;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.FNumber.ValueOffset.Offset = D_BF_DCF_EXIF_O_FNUMBER;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal.ValueOffset.Offset = D_BF_DCF_EXIF_O_DATEORIG;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized.ValueOffset.Offset = D_BF_DCF_EXIF_O_DATEDIGI;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.CompressedBitsPerPixel.ValueOffset.Offset = D_BF_DCF_EXIF_O_BITSPERPIXEL;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.ShutterSpeedValue.ValueOffset.Offset = D_BF_DCF_EXIF_O_SHUTTER;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.ApertureValue.ValueOffset.Offset = D_BF_DCF_EXIF_O_APERTUE;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureBiasValue.ValueOffset.Offset = D_BF_DCF_EXIF_O_EXPOSUREBIAS;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.MaxApertureValue.ValueOffset.Offset = D_BF_DCF_EXIF_O_MAXAPERTUER;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength.ValueOffset.Offset = D_BF_DCF_EXIF_O_FOCAL;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.ValueOffset.Offset = D_BF_DCF_EXIF_O_MAKERNOTE;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.DigitalZoomRatio.ValueOffset.Offset = D_BF_DCF_EXIF_O_DIGIZOOM;

	g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer.ValueOffset.LValue = D_BF_DCF_EXIF_O_R98POINTER;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.offset_to_1stIFD = D_BF_DCF_EXIF_NEXT_IFD_POINTER;

#ifdef CO_BF_DCF_IF_SET_GPSTAG
	g_bf_dcf_exif_jpeg_header_second.Gps.GPSLatitude.ValueOffset.Offset = D_BF_DCF_EXIF_O_GPSLATITUDE;
	g_bf_dcf_exif_jpeg_header_second.Gps.GPSLongitude.ValueOffset.Offset = D_BF_DCF_EXIF_O_GPSLONGITUDE;
	g_bf_dcf_exif_jpeg_header_second.Gps.GPSImgDirection.ValueOffset.Offset = D_BF_DCF_EXIF_O_GPSIMGDIRECTION;
#endif

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	g_bf_dcf_exif_jpeg_header_second.Ifd1.Maker1.ValueOffset.Offset = D_BF_DCF_EXIF_O_MAKER1;
	g_bf_dcf_exif_jpeg_header_second.Ifd1.Model1.ValueOffset.Offset = D_BF_DCF_EXIF_O_MODEL1;
#endif
	g_bf_dcf_exif_jpeg_header_second.Ifd1.XResolution.ValueOffset.Offset = D_BF_DCF_EXIF_O_XRES_1ST;
	g_bf_dcf_exif_jpeg_header_second.Ifd1.YResolution.ValueOffset.Offset = D_BF_DCF_EXIF_O_YRES_1ST;
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	g_bf_dcf_exif_jpeg_header_second.Ifd1.DateTime1.ValueOffset.Offset = D_BF_DCF_EXIF_O_DATETIME1;
#endif
	g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormat.ValueOffset.LValue = D_BF_DCF_EXIF_THUMBNAIL_DATA_OFFSET;
}


/**
 * @brief get make EXIF endian
 * @return endian(Little or Big)
 * @note none
 * @attention none
 */
FW_USHORT BF_Dcf_Exif_Get_Exif_Endian(FW_VOID)
{
	return g_bf_dcf_exif_big_or_lit;
}

FW_USHORT BF_Dcf_Exif_Exchange_App_Size(FW_USHORT app_size)
{
	FW_USHORT tmp_short_val = app_size;

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	// CPU Big-endial
	;	// DO NOTHING
#else
	// CPU Little-endial
	dcf_exif_swap_short(&tmp_short_val);
#endif

	return tmp_short_val;
}


FW_VOID dcf_exif_delete_ifd0_tag(FW_USHORT size, FW_USHORT ifd, FW_USHORT tag_id)
{
	dcf_exif_offset_reduce_ifd0(D_BF_DCF_EXIF_T_IFD_SIZE);
	dcf_exif_offset_reduce_exif(D_BF_DCF_EXIF_T_IFD_SIZE);
	dcf_exif_offset_reduce_ifd1(D_BF_DCF_EXIF_T_IFD_SIZE);

	dcf_exif_offset_reduce_value(size, ifd, tag_id);
}

FW_VOID dcf_exif_delete_exif_tag(FW_USHORT size, FW_USHORT ifd, FW_USHORT tag_id)
{
	dcf_exif_offset_reduce_exif(D_BF_DCF_EXIF_T_IFD_SIZE);
	dcf_exif_offset_reduce_ifd1(D_BF_DCF_EXIF_T_IFD_SIZE);

	dcf_exif_offset_reduce_value(size, ifd, tag_id);
}

FW_VOID dcf_exif_delete_r98_tag(FW_USHORT size, FW_USHORT ifd, FW_USHORT tag_id)
{
	g_bf_dcf_exif_jpeg_header_first.Ifd0.offset_to_1stIFD -= D_BF_DCF_EXIF_T_IFD_SIZE;
	dcf_exif_offset_reduce_ifd1(D_BF_DCF_EXIF_T_IFD_SIZE);

	dcf_exif_offset_reduce_value(size, ifd, tag_id);
}

#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
FW_VOID dcf_exif_delete_ifd1_tag(FW_USHORT size, FW_USHORT ifd, FW_USHORT tag_id)
{
//	dcf_exif_offset_reduce_ifd0(D_BF_DCF_EXIF_T_IFD_SIZE);
//	dcf_exif_offset_reduce_exif(D_BF_DCF_EXIF_T_IFD_SIZE);
	dcf_exif_offset_reduce_ifd1(D_BF_DCF_EXIF_T_IFD_SIZE);

	dcf_exif_offset_reduce_value(size, ifd, tag_id);
}
#endif

FW_VOID dcf_exif_offset_reduce_ifd0(FW_USHORT size)
{
	g_bf_dcf_exif_jpeg_header_first.Ifd0.ImageDescription.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Make.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Model.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.XResolution.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.YResolution.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Software.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Copyright.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.ExifIFDPointer.ValueOffset.LValue -= size;
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	g_bf_dcf_exif_jpeg_header_first.Ifd0.PrintIM.ValueOffset.Offset -= size;
#endif
}

FW_VOID dcf_exif_offset_reduce_exif(FW_USHORT size)
{
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureTime.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.FNumber.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.CompressedBitsPerPixel.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.ShutterSpeedValue.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.ApertureValue.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureBiasValue.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.MaxApertureValue.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.DigitalZoomRatio.ValueOffset.Offset -= size;

	g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer.ValueOffset.LValue -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.offset_to_1stIFD -= size;
}

FW_VOID dcf_exif_offset_reduce_ifd1(FW_USHORT size)
{
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	g_bf_dcf_exif_jpeg_header_second.Ifd1.Maker1.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_second.Ifd1.Model1.ValueOffset.Offset -= size;
#endif
	g_bf_dcf_exif_jpeg_header_second.Ifd1.XResolution.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_second.Ifd1.YResolution.ValueOffset.Offset -= size;
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	g_bf_dcf_exif_jpeg_header_second.Ifd1.DateTime1.ValueOffset.Offset -= size;
#endif
	g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormat.ValueOffset.LValue -= size;
}

FW_UCHAR dcf_exif_offset_reduce_value(FW_USHORT size, FW_USHORT ifd, FW_USHORT tag_id)
{
	g_bf_dcf_exif_jpeg_header_second.Ifd1.JPEGInterchangeFormat.ValueOffset.LValue -= size;
	if ( (ifd == FJ_IFD_1) && (tag_id == FJ_IFD1_TAG_DATETIME1_) ) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_second.Ifd1.DateTime1.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_second.Ifd1.YResolution.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_second.Ifd1.XResolution.ValueOffset.Offset -= size;
	if ( (ifd == FJ_IFD_1) && (tag_id == FJ_IFD1_TAG_MODEL1_) ) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_second.Ifd1.Model1.ValueOffset.Offset -= size;
	if ( (ifd == FJ_IFD_1) && (tag_id == FJ_IFD1_TAG_MAKER1_) ) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_second.Ifd1.Maker1.ValueOffset.Offset -= size;

	g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer.ValueOffset.LValue -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.offset_to_1stIFD -= size;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.MakerNote.ValueOffset.Offset -= size;

	if (tag_id == FJ_EXIF_TAG_MAKERNOTE_) {
		return 0;
	}
	if (tag_id == FJ_EXIF_TAG_DZOOM_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.DigitalZoomRatio.ValueOffset.Offset -= size;
	if (tag_id == FJ_EXIF_TAG_FOCALLEN_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.FocalLength.ValueOffset.Offset -= size;
	if (tag_id == FJ_EXIF_TAG_MAXAPTURE_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.MaxApertureValue.ValueOffset.Offset -= size;
	if (tag_id == FJ_EXIF_TAG_EXPBIAS_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureBiasValue.ValueOffset.Offset -= size;
	if (tag_id == FJ_EXIF_TAG_APERTURE_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.ApertureValue.ValueOffset.Offset -= size;
	if (tag_id == FJ_EXIF_TAG_SHUTTER_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.ShutterSpeedValue.ValueOffset.Offset -= size;
	if (tag_id == FJ_EXIF_TAG_COMPRESSBPP_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.CompressedBitsPerPixel.ValueOffset.Offset -= size;
	if (tag_id == FJ_EXIF_TAG_DATETIMEDITI_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized.ValueOffset.Offset -= size;
	if (tag_id == FJ_EXIF_TAG_DATETIMEORIG_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal.ValueOffset.Offset -= size;
	if (tag_id == FJ_EXIF_TAG_FNUM_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.FNumber.ValueOffset.Offset -= size;
	if (tag_id == FJ_EXIF_TAG_EXPTIME_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExposureTime.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.ExifIFDPointer.ValueOffset.LValue -= size;

	if (tag_id == FJ_IFD0_TAG_PRINTIM_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.Ifd0.PrintIM.ValueOffset.Offset -= size;
	if (tag_id == FJ_IFD0_TAG_COPYRIGHT_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Copyright.ValueOffset.Offset -= size;
	if ( (ifd == FJ_IFD_0) && (tag_id == FJ_IFD0_TAG_DATETIME_) ) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime.ValueOffset.Offset -= size;
	if (tag_id == FJ_IFD0_TAG_SOFT_) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Software.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.YResolution.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.XResolution.ValueOffset.Offset -= size;
	if ( (ifd == FJ_IFD_0) && (tag_id == FJ_IFD0_TAG_MODEL_) ) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Model.ValueOffset.Offset -= size;
	if ( (ifd == FJ_IFD_0) && (tag_id == FJ_IFD0_TAG_MAKE_) ) {
		return 0;
	}
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Make.ValueOffset.Offset -= size;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.ImageDescription.ValueOffset.Offset -= size;
	return 0;
}

FW_VOID dcf_exif_set_tag_offset_thm(FW_VOID)
{
	g_bf_dcf_exif_jpeg_header_first.Ifd0_Num = D_BF_DCF_EXIF_IFD_NUM_OF_IFD0_THM;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num = D_BF_DCF_EXIF_IFD_NUM_OF_IFDEXIF_THM;
	g_bf_dcf_exif_jpeg_header_second.R98Ifd_Num = D_BF_DCF_EXIF_IFD_NUM_OF_R98_THM;

	g_bf_dcf_exif_jpeg_header_first.Ifd0.Make.ValueOffset.Offset = D_BF_DCF_EXIF_O_MAKE_THM;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.Model.ValueOffset.Offset = D_BF_DCF_EXIF_O_MODEL_THM;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.XResolution.ValueOffset.Offset = D_BF_DCF_EXIF_O_XRESOL_THM;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.YResolution.ValueOffset.Offset = D_BF_DCF_EXIF_O_YRESOL_THM;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime.ValueOffset.Offset = D_BF_DCF_EXIF_O_DATETIME_THM;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.ExifIFDPointer.ValueOffset.LValue = D_BF_DCF_EXIF_O_EXIFPOINTER_THM;

	g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal.ValueOffset.Offset = D_BF_DCF_EXIF_O_DATEORIG_THM;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized.ValueOffset.Offset = D_BF_DCF_EXIF_O_DATEDIGI_THM;

	g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer.ValueOffset.LValue = D_BF_DCF_EXIF_O_R98POINTER_THM;
	g_bf_dcf_exif_jpeg_header_first.Ifd0.offset_to_1stIFD = 0;
	g_bf_dcf_exif_jpeg_header_first.ExifIfd.NextIFDOffset = 0;

}

/**
 * @brief set tag data for write buffer
 * @param i:*tag_data
 * @param o:*tmp_buf
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_exif_set_exif_tag_to_buf(T_BF_DCF_EXIF_IFD* tag_data, FW_CHAR* tmp_buf)
{
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
	FW_INT32 length;
	FW_CHAR* p_tmp_buf;
	FW_USHORT tmp_short_val;
	FW_ULONG tmp_long_val;
#endif

	if(tag_data == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	if(tmp_buf == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
	p_tmp_buf = tmp_buf;

	// copy tag id
	length = sizeof(tag_data->Tag);
	tmp_short_val = tag_data->Tag;
	dcf_exif_swap_short(&tmp_short_val);
	memcpy(p_tmp_buf, &tmp_short_val, length);
	p_tmp_buf += length;

	// copy type
	length = sizeof(tag_data->Type);
	tmp_short_val = tag_data->Type;
	dcf_exif_swap_short(&tmp_short_val);
	memcpy(p_tmp_buf, &tmp_short_val, length);
	p_tmp_buf += length;

	// copy count
	length = sizeof(tag_data->Count);
	tmp_long_val = tag_data->Count;
	dcf_exif_swap_long(&tmp_long_val);
	memcpy(p_tmp_buf, &tmp_long_val, length);
	p_tmp_buf += length;

	// copy offset(value)
	switch(tag_data->Type){
		case D_BF_DCF_EXIF_BYTE_T: // FALL THROUGH
		case D_BF_DCF_EXIF_ASCII_T: // FALL THROUGH
		case D_BF_DCF_EXIF_UNDEFINED_T:
			if (tag_data->Count <= 4) {
				// copy value
				length = sizeof(tag_data->ValueOffset.UValue);
				memcpy(p_tmp_buf, tag_data->ValueOffset.UValue, length);
				p_tmp_buf += length;
			}
			else {
				// copy offset
				length = sizeof(tag_data->ValueOffset.Offset);
				tmp_long_val = tag_data->ValueOffset.Offset;
				dcf_exif_swap_long(&tmp_long_val);
				memcpy(p_tmp_buf, &tmp_long_val, length);
				p_tmp_buf += length;
			}
			break;

		case D_BF_DCF_EXIF_SHORT_T:
			if (tag_data->Count <= 2) {
				// copy value
				length = sizeof(tag_data->ValueOffset.SValue.Value);
				tmp_short_val = tag_data->ValueOffset.SValue.Value;
				dcf_exif_swap_short(&tag_data->ValueOffset.SValue.Value);
				memcpy(p_tmp_buf, &tmp_short_val, length);
				p_tmp_buf += length;

				tmp_short_val = tag_data->ValueOffset.SValue.dummy;
				dcf_exif_swap_short(&tag_data->ValueOffset.SValue.dummy);
				memcpy(p_tmp_buf, &tmp_short_val, length);
				p_tmp_buf += length;
			}
			else {
				// copy offset
				length = sizeof(tag_data->ValueOffset.Offset);
				tmp_long_val = tag_data->ValueOffset.Offset;
				dcf_exif_swap_long(&tmp_long_val);
				memcpy(p_tmp_buf, &tmp_long_val, length);
				p_tmp_buf += length;
			}
			break;

		case D_BF_DCF_EXIF_LONG_T: // FALL THROUGH
		case D_BF_DCF_EXIF_RATIONAL_T: // FALL THROUGH
		case D_BF_DCF_EXIF_SLONG_T: // FALL THROUGH
		case D_BF_DCF_EXIF_SRATIONAL_T:
			// copy offset(value)
			length = sizeof(tag_data->ValueOffset.Offset);
			tmp_long_val = tag_data->ValueOffset.Offset;
			dcf_exif_swap_long(&tmp_long_val);
			memcpy(p_tmp_buf, &tmp_long_val, length);
			p_tmp_buf += length;
			break;

		default:
			return FSDCF_ERRNO_PARAM_FAILED;
	}

#else
	memcpy(tmp_buf, tag_data, sizeof(T_BF_DCF_EXIF_IFD));
#endif

	return 0;
}

/**
 * @brief set tag value for write buffer
 * @param i:*tag_val
 * @param i:type
 * @param i:count
 * @param o:*tmp_buf
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_exif_set_exif_data_to_buf(FW_CHAR* tag_val, FW_UCHAR type, FW_UCHAR count, FW_CHAR* tmp_buf)
{
	FW_INT32 length;
	FW_USHORT cnt;
	FW_CHAR* p_tmp_buf;
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
	FW_USHORT tmp_short_val;
	FW_ULONG tmp_long_val;
#endif

	if(tag_val == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	if(tmp_buf == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	p_tmp_buf = tmp_buf;

	switch(type){
		case D_BF_DCF_EXIF_BYTE_T: // FALL THROUGH
		case D_BF_DCF_EXIF_ASCII_T: // FALL THROUGH
		case D_BF_DCF_EXIF_UNDEFINED_T:
			length = 1;
			for(cnt = 0; cnt < count; cnt++){
				memcpy(p_tmp_buf, tag_val, length);
				tag_val += length;
				p_tmp_buf += length;
			}
			break;

		case D_BF_DCF_EXIF_SHORT_T:
			length = 2;
			for(cnt = 0; cnt < count; cnt++){
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
				tmp_short_val = tag_val;
				dcf_exif_swap_short(&tmp_short_val);
				memcpy(p_tmp_buf, &tmp_short_val, length);
#else
				memcpy(p_tmp_buf, tag_val, length);
#endif
				tag_val += length;
				p_tmp_buf += length;
			}
			break;

		case D_BF_DCF_EXIF_LONG_T: // FALL THROUGH
		case D_BF_DCF_EXIF_SLONG_T:
			length = 4;
			for(cnt = 0; cnt < count; cnt++){
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
				tmp_long_val = tag_val;
				dcf_exif_swap_long(&tmp_long_val);
				memcpy(p_tmp_buf, &tmp_long_val, length);
#else
				memcpy(p_tmp_buf, tag_val, length);
#endif
				tag_val += length;
				p_tmp_buf += length;
			}
			break;

		case D_BF_DCF_EXIF_RATIONAL_T: // FALL THROUGH
		case D_BF_DCF_EXIF_SRATIONAL_T:
			length = 4;
			for(cnt = 0; cnt < (FW_USHORT)(count * 2); cnt++){
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
				tmp_long_val = tag_val;
				dcf_exif_swap_long(&tmp_long_val);
				memcpy(p_tmp_buf, &tmp_long_val, length);
#else
				memcpy(p_tmp_buf, tag_val, length);
#endif
				tag_val += length;
				p_tmp_buf += length;
			}
			break;

		default:
			return FSDCF_ERRNO_PARAM_FAILED;
	}

	return 0;
}

/**
 * @brief set tifff header to write buffer
 * @param i:*tiff_val
 * @param o:*tmp_buf
 * @return error code
 * @note none
 * @attention none
 */
static FW_INT32 dcf_exif_set_exif_tiff_header_to_buf(T_BF_DCF_EXIF_TIFF_HEADER* tiff_val, FW_CHAR* tmp_buf)
{
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
	FW_INT32 length;
	FW_CHAR* p_tmp_buf;
	FW_USHORT tmp_short_val;
	FW_ULONG tmp_long_val;
#endif

	if(tiff_val == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	if(tmp_buf == NULL){
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
	p_tmp_buf = tmp_buf;

	// copy ByteOrder
	length = sizeof(tiff_val->ByteOrder);
	memcpy(p_tmp_buf, &tiff_val->ByteOrder, length);
	p_tmp_buf += length;

	// copy reserved
	length = sizeof(tiff_val->Reserved);
	tmp_short_val = tiff_val->Reserved;
	dcf_exif_swap_short(&tmp_short_val);
	memcpy(p_tmp_buf, &tmp_short_val, length);
	p_tmp_buf += length;

	// copy offset
	length = sizeof(tiff_val->Offset);
	tmp_long_val = tiff_val->Offset;
	dcf_exif_swap_long(&tmp_long_val);
	memcpy(p_tmp_buf, &tmp_long_val, length);
	p_tmp_buf += length;
#else
	memcpy(tmp_buf, tiff_val, D_BF_DCF_EXIF_TIFF_HEADER_S);
#endif

	return 0;
}

FW_VOID BF_Dcf_Exif_Copy_Exif_Thm(FW_CHAR* tmp_buff, FW_ULONG* jpg_head_length)
{
	FW_ULONG length = 0;
	T_BF_DCF_EXIF_JPEG_R98_IFD thm_ifd = {
		/* ExifR98 IFD(ExifR98 Information) */
	/* InteroperabilityIndex   */	{FJ_ITOP_TAG_ITOPINDEX_,   D_BF_DCF_EXIF_ASCII_T,    4,{D_BF_DCF_EXIF_DVAL_INTEROPEINDEX_THM}},/* UValue="THM" */
	/* InteroperabilityVersion */	{FJ_ITOP_TAG_ITOPVERSION_, D_BF_DCF_EXIF_UNDEFINED_T,4,{D_BF_DCF_EXIF_DVAL_INTEROPEVER}},/* UValue="0100" */
									0x00000000 /* Next IFD Offset */
	};
	FW_INT32 ercd;

	if (tmp_buff == NULL) {
		return;
	}
	if (jpg_head_length == NULL) {
		return;
	}

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
#else
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
#endif
#else
#ifdef CO_BF_DCF_EXIF_CHANGE_ENDIAN
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
#endif
#endif

	dcf_exif_set_tag_offset_thm();

	// Header Set Start
	memcpy(tmp_buff, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.Soi, D_BF_DCF_EXIF_SHORT_S);
	length += D_BF_DCF_EXIF_SHORT_S;
	memcpy(tmp_buff + length, (FW_VOID*)&g_bf_dcf_exif_jpeg_header_first.App1, D_BF_DCF_EXIF_APP1_MARKER_S);
	length += D_BF_DCF_EXIF_APP1_MARKER_S;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Tiff_Head, D_BF_DCF_EXIF_TIFF_HEADER_S);
	ercd = dcf_exif_set_exif_tiff_header_to_buf((T_BF_DCF_EXIF_TIFF_HEADER*)&g_bf_dcf_exif_jpeg_header_first.Tiff_Head, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write tiff header err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_TIFF_HEADER_S;

	// IFD0 Tag Set Start
//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_Num, D_BF_DCF_EXIF_SHORT_S);
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_Num, D_BF_DCF_EXIF_SHORT_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 number err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_SHORT_S;

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Make.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.Make, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Make, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 make err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Model.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.Model, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Model, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 Model err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.Orientation, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 Orientation err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.XResolution, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.XResolution, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 XResolution err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.YResolution, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.YResolution, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 YResolution err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.ResolutionUnit, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.ResolutionUnit, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 ResolutionUnit err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 DateTime err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.YCbCrPositioning, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.YCbCrPositioning, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 YCbCrPositioning err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.ExifIFDPointer, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.ExifIFDPointer, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write IFD0 ExifIFDPointer err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0.offset_to_1stIFD, sizeof(FW_ULONG));
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0.offset_to_1stIFD, D_BF_DCF_EXIF_LONG_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 offset_to_1stIFD err: ercd=%d\n", ercd));
#endif
	}
	length += sizeof(FW_ULONG);

	// IFD0 Value Set Start
	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Make.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueMake, D_BF_DCF_EXIF_V_MAKE_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueMake, 
											g_bf_dcf_exif_jpeg_header_first.Ifd0.Make.Type, 
											D_BF_DCF_EXIF_V_MAKE_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValueMake err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_MAKE_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.Model.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueModel, D_BF_DCF_EXIF_V_MODEL_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueModel, 
											g_bf_dcf_exif_jpeg_header_first.Ifd0.Model.Type, 
											D_BF_DCF_EXIF_V_MODEL_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValueModel err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_MODEL_S;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueXResolution, D_BF_DCF_EXIF_RATIONAL_S);
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueXResolution, 
										g_bf_dcf_exif_jpeg_header_first.Ifd0.XResolution.Type, 
										g_bf_dcf_exif_jpeg_header_first.Ifd0.XResolution.Count, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValueXResolution err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_RATIONAL_S;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueYResolution, D_BF_DCF_EXIF_RATIONAL_S);
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueYResolution, 
										g_bf_dcf_exif_jpeg_header_first.Ifd0.YResolution.Type, 
										g_bf_dcf_exif_jpeg_header_first.Ifd0.YResolution.Count, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValueYResolution err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_RATIONAL_S;

	if (g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueDateTime, D_BF_DCF_EXIF_V_DATETIME_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.Ifd0_value.ValueDateTime, 
											g_bf_dcf_exif_jpeg_header_first.Ifd0.DateTime.Type, 
											D_BF_DCF_EXIF_V_DATETIME_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val IFD0 ValueDateTime err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_DATETIME_S;
	}

	// EXIF Tag Set Start
//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num, D_BF_DCF_EXIF_SHORT_S);
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_Num, D_BF_DCF_EXIF_SHORT_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write ExifIFD number err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_SHORT_S;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExifVersion, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ExifVersion, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD ExifVersion err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD DateTimeOriginal err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD DateTimeDigitized err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.ComponentsConfiguration, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ComponentsConfiguration, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD ComponentsConfiguration err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.FlashPixVersion, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.FlashPixVersion, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD FlashPixVersion err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.ColorSpace, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.ColorSpace, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD ColorSpace err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelXDimension, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD PixelXDimension err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.PixelYDimension, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD PixelYDimension err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer, D_BF_DCF_EXIF_T_IFD_SIZE);
		ercd = dcf_exif_set_exif_tag_to_buf((T_BF_DCF_EXIF_IFD*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.InteroperabilityIFDPointer, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write EXIF IFD InteroperabilityIFDPointer err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
	}

//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd.NextIFDOffset, sizeof(FW_ULONG));
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd.NextIFDOffset, D_BF_DCF_EXIF_LONG_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD NextIFDOffset err: ercd=%d\n", ercd));
#endif
	}
	length += sizeof(FW_ULONG);

	// Exif Value Set Start
	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeOriginal, D_BF_DCF_EXIF_V_DATEORIG_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeOriginal, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeOriginal.Type, 
											D_BF_DCF_EXIF_V_DATEORIG_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD ValueDateTimeOriginal err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_DATEORIG_S;
	}

	if (g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized.Tag != 0) {
//		memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeDigitized, D_BF_DCF_EXIF_V_DATEORIG_S);
		ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_first.ExifIfd_value.ValueDateTimeDigitized, 
											g_bf_dcf_exif_jpeg_header_first.ExifIfd.DateTimeDigitized.Type, 
											D_BF_DCF_EXIF_V_DATEORIG_S, tmp_buff + length);
		if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("EXIF write val EXIF IFD ValueDateTimeDigitized err: ercd=%d\n", ercd));
#endif
		}
		length += D_BF_DCF_EXIF_V_DATEORIG_S;
	}

	// R98IFD Set Start
//	memcpy(tmp_buff + length, &g_bf_dcf_exif_jpeg_header_second.R98Ifd_Num, D_BF_DCF_EXIF_SHORT_S);
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&g_bf_dcf_exif_jpeg_header_second.R98Ifd_Num, D_BF_DCF_EXIF_SHORT_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val R98 IFD number err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_SHORT_S;

//	if (thm_ifd.InteroperabilityIndex.Tag != 0) {
//		memcpy(tmp_buff + length, &thm_ifd.InteroperabilityIndex, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf(&thm_ifd.InteroperabilityIndex, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write R98 IFD InteroperabilityIndex err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;
//	}

//	if (thm_ifd.InteroperabilityVersion.Tag != 0) {
//		memcpy(tmp_buff + length, &thm_ifd.InteroperabilityVersion, D_BF_DCF_EXIF_T_IFD_SIZE);
	ercd = dcf_exif_set_exif_tag_to_buf(&thm_ifd.InteroperabilityVersion, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write R98 IFD InteroperabilityVersion err: ercd=%d\n", ercd));
#endif
	}
	length += D_BF_DCF_EXIF_T_IFD_SIZE;
//	}

//	memcpy(tmp_buff + length, &thm_ifd.NextIFDOffset, sizeof(FW_ULONG));
	ercd = dcf_exif_set_exif_data_to_buf((FW_CHAR*)&thm_ifd.NextIFDOffset, D_BF_DCF_EXIF_LONG_T, 1, tmp_buff + length);
	if(ercd != FSDCF_ERRNO_OK){
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("EXIF write val R98 IFD NextIFDOffset err: ercd=%d\n", ercd));
#endif
	}
	length += sizeof(FW_ULONG);

	*jpg_head_length = length;

	dcf_exif_init_tag_offset();
}
#endif

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
//----------------------------------------------------------------------------
// for MPF
//----------------------------------------------------------------------------
FW_VOID BF_Dcf_Exif_Exchange_Endian_Mpf(FW_UCHAR mpf_index, FW_USHORT endian)
{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	// big endian
	if (endian == D_BF_DCF_EXIF_LIT_EN) {
		g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
		dcf_exif_exchange_endian_mpf(mpf_index);
	}
#else
	// little endian
	if (endian == D_BF_DCF_EXIF_BIG_EN) {
		g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
		dcf_exif_exchange_endian_mpf(mpf_index);
	}
#endif
}

FW_INT32 BF_Dcf_Exif_Analyze_Mpf_Resize(FW_UCHAR mpf_index, FW_INT32 file_id, FW_ULONG mpf_offset)
{
	FW_INT32  ercd, ercd2;
	FW_UCHAR* buffer;
	FW_ULONG  result;
	FW_ULONG  tag_offset = 0;
	FW_ULONG  reset_offset = 0;
	FW_ULONG  value_offset = 0;
	FW_ULONG  total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
	FW_UCHAR  tag_type;
	FW_ULONG  tag_count;
	FW_ULONG  data_offset;
	FW_USHORT jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
	FW_USHORT zero_ifd_offset = D_BF_DCF_EXIF_TIFFHEADSIZE;

#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
#endif

	// memory allocation
	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&buffer);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	ercd = BF_Mdf_Wrapper_Lseek(file_id, mpf_offset, D_BF_MDF_WRAPPER_SEEK_SET, &result);
	if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		ercd2 = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)buffer);
		if (ercd2 != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d \n", ercd2));
#endif
			ercd = ercd2;
		}
		return ercd;
	}

	while (1) {
		// read data from Exif
		// file pointer : mpf monitor top
		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		if (*(buffer + 3) != 0xE1) {
		// check APP1 marker
		// APP1 marker's place is next to SOI marker
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND APP1 MARKER\n"));
#endif
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;
		}

		// check endian
		g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));

		zero_ifd_offset = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);

		tag_offset = jpeg_start_offset + zero_ifd_offset + 2;
		reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
		gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
		gTagCheckCnt = 1;	// reset seearch count

		// Get XResolution
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_XRES_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
		}
		else if (ercd != 0) {
			break;
		}
		else {
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset) + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer,
											   (FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_value.ValueXResolution.Num,
											   (FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_value.ValueXResolution.Den);
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Tag = FJ_IFD1_TAG_XRES_;
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num++;
		}
		tag_offset = reset_offset;

		// Get YResolution
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_YRES_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
		}
		else if (ercd != 0) {
			break;
		}
		else {
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset) + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
			ercd = dcf_exif_get_exif_set_value(file_id, buffer, &data_offset, total_buffer,
											   (FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_value.ValueYResolution.Num,
											   (FW_ULONG*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_value.ValueYResolution.Den);
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Tag = FJ_IFD1_TAG_YRES_;
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num++;
		}
		tag_offset = reset_offset;

		// Get ResolutionUnit
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_RESUNIT_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
		}
		else if (ercd != 0) {
			break;
		}
		else {
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.ValueOffset.SValue.Value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.Tag = FJ_IFD1_TAG_RESUINT_;
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num++;
		}
		tag_offset = reset_offset;

		// Get EXIF IDF offset
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			break;
		}
		else if (ercd != 0) {
			break;
		}
		else {
		}
		data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

		// get next start offset
		ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
		if (ercd != 0) {
			break;
		}

#if 0
		// maker note
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_MAKERNOTE_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
		}
		else if (ercd != 0) {
			break;
		}
		else {
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Count = tag_count;
		}
		tag_offset = reset_offset;
#endif

#if 0
		// image width
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELXDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
		}
		else if (ercd != 0) {
		}
		else {
			if (tag_type == 3) {
				g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.ValueOffset.SValue.Value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
				g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.ValueOffset.SValue.dummy = 0;
				g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Type = D_BF_DCF_EXIF_SHORT_T;
			}
			else {
				g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.ValueOffset.LValue = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Type = D_BF_DCF_EXIF_LONG_T;
			}
		}
		tag_offset = reset_offset;

		// image height
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_PEXELYDIM_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
		}
		else if (ercd != 0) {
		}
		else {
			if (tag_type == 3) {
				g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.ValueOffset.SValue.Value = dcf_exif_get_short_value(buffer + tag_offset + value_offset);
				g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.ValueOffset.SValue.dummy = 0;
				g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Type = D_BF_DCF_EXIF_SHORT_T;
			}
			else {
				g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.ValueOffset.LValue = dcf_exif_get_long_value(buffer + tag_offset + value_offset);
				g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Type = D_BF_DCF_EXIF_LONG_T;
			}
		}
		tag_offset = reset_offset;
#endif

		// Image Unique ID
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_IMAGEUID_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			ercd = 0;
		}
		else if (ercd != 0) {
		}
		else {
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset) + (total_buffer - D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE);
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Tag = FJ_EXIF_TAG_IMAGEUID_;
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Count = tag_count;
			memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].image_uid_value, (void*)(buffer + data_offset), tag_count);
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num++;
		}

		break;
	}

	ercd2 = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)buffer);
	if (ercd2 != D_BF_FS_USER_CUSTOM_OK) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd2));
#endif
		ercd = ercd2;
	}

	// reset file pointer
	ercd2 = BF_Mdf_Wrapper_Lseek(file_id, 0, D_BF_MDF_WRAPPER_SEEK_SET, &result);
	if (ercd2 != 0) {
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
		ercd = ercd2;
	}

	return ercd;
}

static FW_VOID dcf_exif_exchange_endian_mpf_app2(FW_VOID);
static FW_VOID dcf_exif_exchange_endian_mpf_entry(T_BF_DCF_EXIF_MP_ENTRY* mp_entry);
#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
static FW_VOID dcf_exif_exchange_endian_mpf_extended_first_app2(FW_VOID);
static FW_VOID dcf_exif_exchange_endian_mpf_extended_app2(FW_VOID);
#endif // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

static FW_VOID dcf_exif_exchange_endian_mpf_app2(FW_VOID)
{
	g_bf_dcf_exif_mpf_app2.mp_head.ByteOrder = g_bf_dcf_exif_big_or_lit;
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_app2.mp_head.Reserved);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_app2.mp_head.Offset);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd_num);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_format_version.Tag);
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_format_version.Type);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_format_version.Count);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry_num.Tag);
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry_num.Type);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry_num.Count);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry_num.ValueOffset.LValue);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry.Tag);
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry.Type);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry.Count);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry.ValueOffset.Offset);
}

static FW_VOID dcf_exif_exchange_endian_mpf_entry(T_BF_DCF_EXIF_MP_ENTRY* mp_entry)
{
	dcf_exif_swap_long(&(mp_entry->mp_info));
	dcf_exif_swap_long(&(mp_entry->mp_data_size));
	dcf_exif_swap_long(&(mp_entry->mp_data_offset));
	dcf_exif_swap_short(&(mp_entry->mp_entry_1));
	dcf_exif_swap_short(&(mp_entry->mp_entry_2));
}

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
static FW_VOID dcf_exif_exchange_endian_mpf_extended_first_app2(FW_VOID)
{
	g_bf_dcf_exif_mpf_ext_first_app2.mp_head.ByteOrder = g_bf_dcf_exif_big_or_lit;
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_head.Reserved);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_head.Offset);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd_num);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_format_version.Tag);
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_format_version.Type);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_format_version.Count);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry_num.Tag);
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry_num.Type);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry_num.Count);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry_num.ValueOffset.LValue);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry.Tag);
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry.Type);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry.Count);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry.ValueOffset.Offset);
	
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.next_ifd_pointer);	

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd_num);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.mp_format_version.Tag);
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.mp_format_version.Type);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.mp_format_version.Count);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.mp_individual_num.Tag);
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.mp_individual_num.Type);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.mp_individual_num.Count);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.mp_individual_num.ValueOffset.LValue);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.base_viewpoint_num.Tag);
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.base_viewpoint_num.Type);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.base_viewpoint_num.Count);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.base_viewpoint_num.ValueOffset.LValue);
}

static FW_VOID dcf_exif_exchange_endian_mpf_extended_app2(FW_VOID)
{
	g_bf_dcf_exif_mpf_ext_app2.mp_head.ByteOrder = g_bf_dcf_exif_big_or_lit;
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_app2.mp_head.Reserved);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_app2.mp_head.Offset);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd_num);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.mp_format_version.Tag);
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.mp_format_version.Type);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.mp_format_version.Count);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.mp_individual_num.Tag);
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.mp_individual_num.Type);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.mp_individual_num.Count);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.mp_individual_num.ValueOffset.LValue);

	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.base_viewpoint_num.Tag);
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.base_viewpoint_num.Type);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.base_viewpoint_num.Count);
	dcf_exif_swap_long((FW_ULONG*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.base_viewpoint_num.ValueOffset.LValue);
}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

FW_VOID BF_Dcf_Exif_Exchange_Endian_Mpf_Entry(FW_USHORT endian, T_BF_DCF_EXIF_MP_ENTRY* mp_entry)
{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	// big endian
	if (endian == D_BF_DCF_EXIF_LIT_EN) {
		g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
		dcf_exif_exchange_endian_mpf_entry(mp_entry);
	}
#else
	// little endian
	if (endian == D_BF_DCF_EXIF_BIG_EN) {
		g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
		dcf_exif_exchange_endian_mpf_entry(mp_entry);
	}
#endif
}

FW_VOID BF_Dcf_Exif_Exchange_Endian_Mpf_App2(FW_USHORT endian)
{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	// big endian
	if (endian == D_BF_DCF_EXIF_LIT_EN) {
		g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
		dcf_exif_exchange_endian_mpf_app2();
	}
#else
	// little endian
	dcf_exif_swap_short((FW_USHORT*)&g_bf_dcf_exif_mpf_app2.app2.field_length);

	if (endian == D_BF_DCF_EXIF_BIG_EN) {
		g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
		dcf_exif_exchange_endian_mpf_app2();
	}
#endif
}

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
FW_VOID BF_Dcf_Exif_Exchange_Endian_Mpf_Extended_First_App2(FW_USHORT endian)
{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	// big endian
	if (endian == D_BF_DCF_EXIF_LIT_EN) {
		g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
		dcf_exif_exchange_endian_mpf_extended_first_app2();
	}
#else
	// little endian
	if (endian == D_BF_DCF_EXIF_BIG_EN) {
		g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
		dcf_exif_exchange_endian_mpf_extended_first_app2();
	}
#endif
}

FW_VOID BF_Dcf_Exif_Exchange_Endian_Mpf_Extended_App2(FW_USHORT endian)
{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	// big endian
	if (endian == D_BF_DCF_EXIF_LIT_EN) {
		g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
		dcf_exif_exchange_endian_mpf_extended_app2();
	}
#else
	// little endian
	if (endian == D_BF_DCF_EXIF_BIG_EN) {
		g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
		dcf_exif_exchange_endian_mpf_extended_app2();
	}
#endif
}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

FW_VOID BF_Dcf_Exif_Reset_App2_Mpf(FW_VOID)
{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	g_bf_dcf_exif_mpf_app2.mp_head.ByteOrder = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_mpf_app2.mp_head.ByteOrder = D_BF_DCF_EXIF_LIT_EN;
#endif
	g_bf_dcf_exif_mpf_app2.mp_head.Reserved = D_BF_DCF_EXIF_MP_HEAD_RESERVED;
	g_bf_dcf_exif_mpf_app2.mp_head.Offset   = D_BF_DCF_EXIF_MP_HEAD_OFFSET;

	g_bf_dcf_exif_mpf_app2.mp_index_ifd_num = D_BF_DCF_EXIF_MP_INDEX_IFD_NUM;

	g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_format_version.Tag   = 0xB000;
	g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_format_version.Type  = D_BF_DCF_EXIF_UNDEFINED_T;
	g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_format_version.Count = 4;

	g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry_num.Tag   = 0xB001;
	g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry_num.Type  = D_BF_DCF_EXIF_LONG_T;
	g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry_num.Count = 1;

	g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry.Tag                = 0xB002;
	g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry.Type               = D_BF_DCF_EXIF_UNDEFINED_T;
	g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry.ValueOffset.Offset = D_BF_DCF_EXIF_MP_ENTRY_OFFSET;
}

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
FW_VOID BF_Dcf_Exif_Reset_Extended_First_App2_Mpf(FW_VOID)
{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	g_bf_dcf_exif_mpf_ext_first_app2.mp_head.ByteOrder = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_mpf_ext_first_app2.mp_head.ByteOrder = D_BF_DCF_EXIF_LIT_EN;
#endif
	g_bf_dcf_exif_mpf_ext_first_app2.mp_head.Reserved = D_BF_DCF_EXIF_MP_HEAD_RESERVED;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_head.Offset   = D_BF_DCF_EXIF_MP_HEAD_OFFSET;

	g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd_num = D_BF_DCF_EXIF_MP_INDEX_IFD_NUM;

	g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_format_version.Tag   = 0xB000;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_format_version.Type  = D_BF_DCF_EXIF_UNDEFINED_T;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_format_version.Count = 4;

	g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry_num.Tag   = 0xB001;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry_num.Type  = D_BF_DCF_EXIF_LONG_T;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry_num.Count = 1;

	g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry.Tag                = 0xB002;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry.Type               = D_BF_DCF_EXIF_UNDEFINED_T;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry.ValueOffset.Offset = D_BF_DCF_EXIF_MP_ENTRY_OFFSET;

	g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd_num = D_BF_DCF_EXIF_EXT_MP_ATTRIBUTE_IFD_NUM;

	g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.mp_format_version.Tag   = 0xB000;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.mp_format_version.Type  = D_BF_DCF_EXIF_UNDEFINED_T;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.mp_format_version.Count = 4;
	
	g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.mp_individual_num.Tag   = 0xB101;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.mp_individual_num.Type  = D_BF_DCF_EXIF_LONG_T;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.mp_individual_num.Count = 1;
	
	g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.base_viewpoint_num.Tag   = 0xB204;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.base_viewpoint_num.Type  = D_BF_DCF_EXIF_LONG_T;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd.base_viewpoint_num.Count = 1;
}

FW_VOID BF_Dcf_Exif_Reset_Extended_App2_Mpf(FW_VOID)
{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	g_bf_dcf_exif_mpf_ext_app2.mp_head.ByteOrder = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_mpf_ext_app2.mp_head.ByteOrder = D_BF_DCF_EXIF_LIT_EN;
#endif
	g_bf_dcf_exif_mpf_ext_app2.mp_head.Reserved = D_BF_DCF_EXIF_MP_HEAD_RESERVED;
	g_bf_dcf_exif_mpf_ext_app2.mp_head.Offset   = D_BF_DCF_EXIF_MP_HEAD_OFFSET;

	g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd_num = D_BF_DCF_EXIF_EXT_MP_ATTRIBUTE_IFD_NUM;

	g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.mp_format_version.Tag   = 0xB000;
	g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.mp_format_version.Type  = D_BF_DCF_EXIF_UNDEFINED_T;
	g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.mp_format_version.Count = 4;
	
	g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.mp_individual_num.Tag   = 0xB101;
	g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.mp_individual_num.Type  = D_BF_DCF_EXIF_LONG_T;
	g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.mp_individual_num.Count = 1;
	
	g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.base_viewpoint_num.Tag   = 0xB204;
	g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.base_viewpoint_num.Type  = D_BF_DCF_EXIF_LONG_T;
	g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd.base_viewpoint_num.Count = 1;
}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

FW_VOID BF_Dcf_Exif_Copy_App2_Mpf(FW_CHAR* tmp_buff, FW_ULONG* mpf_app2_length, FW_ULONG* mp_entry_offset, FW_USHORT mp_entry_num, FW_USHORT endian)
{
	FW_ULONG length;

	if (tmp_buff == NULL) {
		return;
	}
	if (mpf_app2_length == NULL) {
		return;
	}

	g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry_num.ValueOffset.LValue = mp_entry_num;
	g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry.Count                  = (mp_entry_num * 16);
	g_bf_dcf_exif_mpf_app2.app2.field_length                            = sizeof(T_BF_DCF_EXIF_APP2_MARKER_MPF) + sizeof(T_BF_DCF_EXIF_MP_HEADER) + sizeof(DCF_SHORT) + sizeof(T_BF_DCF_EXIF_MPF_INDEX_IFD) + g_bf_dcf_exif_mpf_app2.mp_index_ifd.mp_entry.Count - 2;

#if 1
	BF_Dcf_Exif_Exchange_Endian_Mpf_App2(endian);
#endif

	memcpy(tmp_buff, (FW_VOID*)&g_bf_dcf_exif_mpf_app2.app2, sizeof(T_BF_DCF_EXIF_APP2_MARKER_MPF));
	length = sizeof(T_BF_DCF_EXIF_APP2_MARKER_MPF);
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_app2.mp_head, sizeof(T_BF_DCF_EXIF_MP_HEADER));
	length += sizeof(T_BF_DCF_EXIF_MP_HEADER);
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd_num, sizeof(DCF_SHORT));
	length += sizeof(DCF_SHORT);
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_app2.mp_index_ifd, sizeof(T_BF_DCF_EXIF_MPF_INDEX_IFD));
	length += sizeof(T_BF_DCF_EXIF_MPF_INDEX_IFD);

	*mp_entry_offset = length;

	memset((tmp_buff + length), 0, (mp_entry_num * 16));
	length += (mp_entry_num * 16);

	*mpf_app2_length = length;
}

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
FW_VOID BF_Dcf_Exif_Copy_Extended_First_App2_Mpf(FW_CHAR* tmp_buff, FW_ULONG* mpf_app2_length, FW_ULONG* mp_entry_offset, FW_USHORT mp_entry_num, FW_USHORT endian)
{
	FW_ULONG length;

	if (tmp_buff == NULL) {
		return;
	}
	if (mpf_app2_length == NULL) {
		return;
	}

	g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry_num.ValueOffset.LValue = mp_entry_num;
	g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry.Count                  = (mp_entry_num * 16);
	g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.next_ifd_pointer		     = D_BF_DCF_EXIF_MP_ENTRY_OFFSET + (mp_entry_num * 16);
	g_bf_dcf_exif_mpf_ext_first_app2.app2.field_length                            = sizeof(T_BF_DCF_EXIF_APP2_MARKER_MPF) + sizeof(T_BF_DCF_EXIF_MP_HEADER) + sizeof(DCF_SHORT) + sizeof(T_BF_DCF_EXIF_MPF_INDEX_IFD) + sizeof(DCF_SHORT) + sizeof(T_BF_DCF_EXIF_MPF_MP_ATTRIBUTES_IFD) + g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd.mp_entry.Count - 2;

#if 1
	BF_Dcf_Exif_Exchange_Endian_Mpf_Extended_First_App2(endian);
#endif

	memcpy(tmp_buff, (FW_VOID*)&g_bf_dcf_exif_mpf_ext_first_app2.app2, sizeof(T_BF_DCF_EXIF_APP2_MARKER_MPF));
	length = sizeof(T_BF_DCF_EXIF_APP2_MARKER_MPF);
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_head, sizeof(T_BF_DCF_EXIF_MP_HEADER));
	length += sizeof(T_BF_DCF_EXIF_MP_HEADER);
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd_num, sizeof(DCF_SHORT));
	length += sizeof(DCF_SHORT);
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_index_ifd, sizeof(T_BF_DCF_EXIF_MPF_INDEX_IFD));
	length += sizeof(T_BF_DCF_EXIF_MPF_INDEX_IFD);

	*mp_entry_offset = length;

	memset((tmp_buff + length), 0, (mp_entry_num * 16));
	length += (mp_entry_num * 16);
	
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd_num, sizeof(DCF_SHORT));
	length += sizeof(DCF_SHORT);
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_ext_first_app2.mp_attribute_ifd, sizeof(T_BF_DCF_EXIF_MPF_MP_ATTRIBUTES_IFD));
	length += sizeof(T_BF_DCF_EXIF_MPF_MP_ATTRIBUTES_IFD);

	*mpf_app2_length = length;
}

FW_VOID BF_Dcf_Exif_Copy_Extended_App2_Mpf(FW_CHAR* tmp_buff, FW_ULONG* mpf_app2_length, FW_USHORT endian)
{
	FW_ULONG length;

	if (tmp_buff == NULL) {
		return;
	}
	if (mpf_app2_length == NULL) {
		return;
	}

	g_bf_dcf_exif_mpf_ext_app2.app2.field_length                            = sizeof(T_BF_DCF_EXIF_APP2_MARKER_MPF) + sizeof(T_BF_DCF_EXIF_MP_HEADER) + sizeof(DCF_SHORT) + sizeof(T_BF_DCF_EXIF_MPF_MP_ATTRIBUTES_IFD) - 2;

#if 1
	BF_Dcf_Exif_Exchange_Endian_Mpf_Extended_App2(endian);
#endif

	memcpy(tmp_buff, (FW_VOID*)&g_bf_dcf_exif_mpf_ext_app2.app2, sizeof(T_BF_DCF_EXIF_APP2_MARKER_MPF));
	length = sizeof(T_BF_DCF_EXIF_APP2_MARKER_MPF);
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_ext_app2.mp_head, sizeof(T_BF_DCF_EXIF_MP_HEADER));
	length += sizeof(T_BF_DCF_EXIF_MP_HEADER);
	
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd_num, sizeof(DCF_SHORT));
	length += sizeof(DCF_SHORT);
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_ext_app2.mp_attribute_ifd, sizeof(T_BF_DCF_EXIF_MPF_MP_ATTRIBUTES_IFD));
	length += sizeof(T_BF_DCF_EXIF_MPF_MP_ATTRIBUTES_IFD);

	*mpf_app2_length = length;
}
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

#if 1

FW_VOID BF_Dcf_Exif_Get_App1_Size_Mpf(FW_UCHAR mpf_index, FW_USHORT* mpf_app1_size)
{
	FW_ULONG length = 0;

	length += D_BF_DCF_EXIF_SHORT_S;			// SOI
	length += D_BF_DCF_EXIF_APP1_MARKER_S;	// APP1 Marker
	length += D_BF_DCF_EXIF_TIFF_HEADER_S;	// TIFF Header
	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num != 0) {
		length += D_BF_DCF_EXIF_SHORT_S;		// IFD0 NUM
		length += (D_BF_DCF_EXIF_T_IFD_SIZE * g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num);
		length += sizeof(FW_ULONG);	// Next IFD Offset

		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Tag != 0) {
			length += D_BF_DCF_EXIF_RATIONAL_S;
		}
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Tag != 0) {
			length += D_BF_DCF_EXIF_RATIONAL_S;
		}
	}
	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num != 0) {
		length += D_BF_DCF_EXIF_SHORT_S;		// Exif-IFD NUM
		length += (D_BF_DCF_EXIF_T_IFD_SIZE * g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num);
		length += sizeof(FW_ULONG);	// Next IFD Offset

		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Tag != 0) {
			length += g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Count;
		}
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Tag != 0) {
			length += D_BF_DCF_EXIF_IMAGEUID_SIZE;
		}
	}
	else {
		length -= D_BF_DCF_EXIF_T_IFD_SIZE;		// reduce Exif_IFD_Pointer's size of IFD0
	}
	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num != 0) {
		length += D_BF_DCF_EXIF_SHORT_S;		// IFD1 NUM
		length += (D_BF_DCF_EXIF_T_IFD_SIZE * g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num);
		length += sizeof(FW_ULONG);	// Next IFD Offset
	}

	*mpf_app1_size = length;
}

FW_VOID BF_Dcf_Exif_Make_Exif_Mpg(FW_UCHAR mpf_index, T_BF_DCF_IF_SCREEN_DATA* scr_data)
{
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.ValueOffset.LValue = scr_data->screen_width;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.ValueOffset.LValue = scr_data->screen_lines;

	if ((scr_data->thumb_addr != 0) && (scr_data->thumb_size != 0)) {
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num                                            = D_BF_DCF_EXIF_MPF_IFD1_NUM;
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.Tag                      = FJ_IFD1_TAG_JPG_INTERCHGFMT_;
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.ValueOffset.LValue       = D_BF_DCF_EXIF_MPF_THUMB_OFFSET;
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormatLength.Tag                = FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_;
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormatLength.ValueOffset.LValue = scr_data->thumb_size;
	}
}

#if 1
FW_VOID BF_Dcf_Exif_Copy_Exif_Mpf(FW_UCHAR mpf_index, FW_USHORT endian, FW_CHAR* tmp_buff, FW_ULONG* mpf_head_length)
#else
FW_VOID BF_Dcf_Exif_Copy_Exif_Mpf(FW_UCHAR mpf_index, FW_CHAR* tmp_buff, FW_ULONG* mpf_head_length)
#endif
{
	FW_ULONG length = 0;
#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
	FW_ULONG padding;
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON

	if (tmp_buff == NULL) {
		return;
	}
	if (mpf_head_length == NULL) {
		return;
	}

	dcf_exif_set_offset_mpf(mpf_index);
#if 1
	BF_Dcf_Exif_Exchange_Endian_Mpf(mpf_index, endian);
#endif
	// Header Set
	memcpy(tmp_buff, (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].soi, D_BF_DCF_EXIF_SHORT_S);
	length += D_BF_DCF_EXIF_SHORT_S;
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].app1, D_BF_DCF_EXIF_APP1_MARKER_S);
	length += D_BF_DCF_EXIF_APP1_MARKER_S;
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].tiff_head, D_BF_DCF_EXIF_TIFF_HEADER_S);
	length += D_BF_DCF_EXIF_TIFF_HEADER_S;

	// IFD0 Set
	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num != 0) {
		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num, D_BF_DCF_EXIF_SHORT_S);
		length += D_BF_DCF_EXIF_SHORT_S;

		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}

		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.offset_to_1stIFD, sizeof(FW_ULONG));
		length += sizeof(FW_ULONG);

		// Set IFD0 Value
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_value.ValueXResolution, D_BF_DCF_EXIF_RATIONAL_S);
			length += D_BF_DCF_EXIF_RATIONAL_S;
		}
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_value.ValueYResolution, D_BF_DCF_EXIF_RATIONAL_S);
			length += D_BF_DCF_EXIF_RATIONAL_S;
		}
	}
	else {
		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num, D_BF_DCF_EXIF_SHORT_S);
		length += D_BF_DCF_EXIF_SHORT_S;

		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.offset_to_1stIFD, sizeof(FW_ULONG));
		length += sizeof(FW_ULONG);
	}

	// Exif IFD Set
	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num != 0) {
		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num, D_BF_DCF_EXIF_SHORT_S);
		length += D_BF_DCF_EXIF_SHORT_S;

		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}

		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.NextIFDOffset, sizeof(FW_ULONG));
		length += sizeof(FW_ULONG);

		// Set Exif IFD Value
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Tag != 0) {
			if (mpf_index == 0) {
				if (1 != FJ_Host_CreateMakerNote_Class1_CB((CHAR*)(tmp_buff + length), g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Count, length, NULL, 0)) {
					// Fail
					memset((tmp_buff + length), 0, g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Count);
				}
			}
			else {
				if (1 != FJ_Host_CreateMakerNote_Class2_CB((CHAR*)(tmp_buff + length), g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Count, length, NULL, 0)) {
					// Fail
					memset((tmp_buff + length), 0, g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Count);
				}
			}
			length += g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Count;
		}
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].image_uid_value, D_BF_DCF_EXIF_IMAGEUID_SIZE);
			length += D_BF_DCF_EXIF_IMAGEUID_SIZE;
		}
	}

	// IFD1 Set
	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num != 0) {
		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num, D_BF_DCF_EXIF_SHORT_S);
		length += D_BF_DCF_EXIF_SHORT_S;

		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat, D_BF_DCF_EXIF_T_IFD_SIZE);
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormatLength, D_BF_DCF_EXIF_T_IFD_SIZE);
		length += D_BF_DCF_EXIF_T_IFD_SIZE;

		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.NextIFDOffset, sizeof(FW_ULONG));
		length += sizeof(FW_ULONG);
	}
#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
	padding = (4 - (length % 4));
	if (padding == 4) {
		padding = 0;
	}
	length += padding;
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON

	*mpf_head_length = length;
	BF_Dcf_Exif_Reset_App1_Mpf(mpf_index);
}

FW_VOID BF_Dcf_Exif_Reset_App1_Mpf_Resize(FW_UCHAR mpf_index)
{
#ifdef FJ_CUSTOM_EXIF_CPU_BIG_ENDIAN
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].tiff_head.ByteOrder = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].tiff_head.ByteOrder = D_BF_DCF_EXIF_LIT_EN;
#endif
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].tiff_head.Reserved = D_BF_DCF_EXIF_TIFF_HEARD_RESERVED;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].tiff_head.Offset   = D_BF_DCF_EXIF_TIFF_HEARD_OFFSET;

	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Type  = D_BF_DCF_EXIF_RATIONAL_T;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Count = 1;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Type  = D_BF_DCF_EXIF_RATIONAL_T;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Count = 1;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.Type  = D_BF_DCF_EXIF_SHORT_T;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.Count = 1;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.Type  = D_BF_DCF_EXIF_LONG_T;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.Count = 1;

	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Type  = D_BF_DCF_EXIF_UNDEFINED_T;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Type  = D_BF_DCF_EXIF_LONG_T;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Count = 1;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Type  = D_BF_DCF_EXIF_LONG_T;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Count = 1;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Type  = D_BF_DCF_EXIF_UNDEFINED_T;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Count = D_BF_DCF_EXIF_IMAGEUID_SIZE;

	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.Type  = D_BF_DCF_EXIF_LONG_T;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.Count = 1;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormatLength.Type  = D_BF_DCF_EXIF_LONG_T;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormatLength.Count = 1;
}

FW_VOID BF_Dcf_Exif_Reset_App1_Mpf(FW_UCHAR mpf_index)
{
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num                                 = D_BF_DCF_EXIF_MPF_IFD0_NUM;

	// IFD0 Tag Reset
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Tag                     = 0x0000;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Tag                     = 0x0000;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.Tag                  = 0x0000;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.Tag                  = FJ_IFD0_TAG_EXIFPTR_;

	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.ValueOffset.Offset   = 0x00000000;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.offset_to_1stIFD                    = 0x00000000;

	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num                             = D_BF_DCF_EXIF_MPF_EXIFIFD_NUM;

	// Exif IFD Tag Reset
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Tag                   = 0x0000;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Tag             = FJ_EXIF_TAG_PEXELXDIM_;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Tag             = FJ_EXIF_TAG_PEXELYDIM_;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Tag                    = 0x0000;

	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num                                 = 0;

	// IFD1 Tag Reset
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.Tag           = 0x0000;
	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormatLength.Tag     = 0x0000;
}

/**
 * @brief Get app1 size for screen nail(MPF)
 * @param i:top_addr top address of buffer(SOI maker position of screen nail)
 * @param o:*size app1 size
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Exif_Get_App1_Size_Mpf_For_Buf(FW_UCHAR* top_addr, FW_ULONG* size)
{
	FW_INT32 ercd;

	// check DQT marker
	if (*(top_addr + 3) == 0xDB) {
		// detect JPEG(without APPx) format
		*size = 0;
		ercd = FSDCF_ERRNO_NOT_APP1;
	}
	// check APP1 marker
	// APP1 marker's place is next to SOI marker
	else if (*(top_addr + 3) == 0xE1) {
		// find APP1 maker
		*size = dcf_exif_get_short_value_big_endian(top_addr + 4);
		ercd = FSDCF_ERRNO_OK;
	}
	else {
		// unknown data
		*size = 0;
		ercd = FSDCF_ERRNO_NOT_APP1;
	}

	return ercd;
}

static FW_VOID dcf_exif_set_offset_mpf(FW_UCHAR mpf_index)
{
#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
	FW_ULONG padding;
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON

	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num == 0) {
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.Tag = 0x0000;
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num -= 1;
	}

	g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.ValueOffset.Offset = (D_BF_DCF_EXIF_TIFFHEADSIZE + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num) + 4);
	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num != 0) {
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.offset_to_1stIFD                         = (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.ValueOffset.Offset + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num) + 4);
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.ValueOffset.LValue = (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.offset_to_1stIFD                  + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num)     + 4);
	}
	else {
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.offset_to_1stIFD                         = 0;
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.ValueOffset.LValue = 0;
	}

	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Tag != 0) {
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.ValueOffset.Offset = g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.ValueOffset.Offset;

		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.ValueOffset.Offset += sizeof(T_BF_DCF_EXIF_RATIONAL);
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num != 0) {
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.offset_to_1stIFD                         += sizeof(T_BF_DCF_EXIF_RATIONAL);
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.ValueOffset.LValue += sizeof(T_BF_DCF_EXIF_RATIONAL);
		}
	}

	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Tag != 0) {
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.ValueOffset.Offset = g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.ValueOffset.Offset;

		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.ValueOffset.Offset += sizeof(T_BF_DCF_EXIF_RATIONAL);
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num != 0) {
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.offset_to_1stIFD                         += sizeof(T_BF_DCF_EXIF_RATIONAL);
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.ValueOffset.LValue += sizeof(T_BF_DCF_EXIF_RATIONAL);
		}
	}

	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Tag != 0) {
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.ValueOffset.Offset = (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.ValueOffset.Offset + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num) + 4);

		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num != 0) {
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.offset_to_1stIFD                         += g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Count;
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.ValueOffset.LValue += g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Count;
		}
	}

	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Tag != 0) {
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.ValueOffset.Offset = (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.ValueOffset.Offset + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num) + 4);
		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Tag != 0) {
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.ValueOffset.Offset += g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Count;
		}

		if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num != 0) {
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.offset_to_1stIFD                         += g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Count;
			g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.ValueOffset.LValue += g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Count;
		}
	}

#ifdef CO_BF_DCF_IF_JPG_SAVE_PADD_ON
	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1_num != 0) {
		padding = (4 - (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.ValueOffset.LValue % 4));
		if (padding == 4) {
			padding = 0;
		}
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd1.JPEGInterchangeFormat.ValueOffset.LValue += padding;
	}
#endif	// CO_BF_DCF_IF_JPG_SAVE_PADD_ON

#if 0
	if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num == 0) {
		g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ExifIFDPointer.ValueOffset.Offset = 0x00000000;
	}
#endif
}

FW_INT32 BF_Dcf_Exif_Set_Value_Mpf(FW_UCHAR mpf_index, FW_USHORT ifd, FW_USHORT tag, FW_USHORT type, FW_USHORT count, FW_UCHAR* addr)
{
	switch (type) {
		case 0xFFFF:		// FALL THROUGH
		case D_BF_DCF_EXIF_BYTE_T:		// FALL THROUGH
		case D_BF_DCF_EXIF_ASCII_T:		// FALL THROUGH
		case D_BF_DCF_EXIF_UNDEFINED_T:
			break;

		case D_BF_DCF_EXIF_SHORT_T:
			count *= D_BF_DCF_EXIF_SHORT_S;
			break;

		case D_BF_DCF_EXIF_LONG_T:
			count *= D_BF_DCF_EXIF_LONG_S;
			break;

		case D_BF_DCF_EXIF_RATIONAL_T:	// FALL THROUGH
		case D_BF_DCF_EXIF_SRATIONAL_T:
			count *= D_BF_DCF_EXIF_RATIONAL_S;
			break;

		default:
			return -1;
	}

	if (ifd == FJ_IFD_0) {
		switch (tag) {
			case FJ_IFD0_TAG_XRES_:		// XResolution
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.XResolution.Tag = FJ_IFD0_TAG_XRES_;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num += 1;
					}
					else {
						;	// DO NOTHING
					}

					memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_value.ValueXResolution, addr, count);
				}
				break;

			case FJ_IFD0_TAG_YRES_:		// YResolution
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.YResolution.Tag = FJ_IFD0_TAG_YRES_;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num += 1;
					}
					else {
						;	// DO NOTHING
					}

					memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_value.ValueYResolution, addr, count);
				}
				break;

			case FJ_IFD0_TAG_RESUNIT_:		// ResolutionUnit
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.Tag = FJ_IFD0_TAG_RESUNIT_;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0_num += 1;
					}
					else {
						;	// DO NOTHING
					}

					memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].ifd0.ResolutionUnit.ValueOffset.SValue.Value, addr, count);
				}
				break;

			default:
				// Error
				return -1;
		}
	}
	else if (ifd == FJ_IFD_EXIF) {
		switch (tag) {
			case FJ_EXIF_TAG_MAKERNOTE_:	// MakerNote
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Tag = FJ_EXIF_TAG_MAKERNOTE_;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num += 1;
					}
					else {
						;	// DO NOTHING
					}

					g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.MakerNote.Count = count;
				}
				break;

			case FJ_EXIF_TAG_PEXELXDIM_:	// PixelXDimension
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Tag = FJ_EXIF_TAG_PEXELXDIM_;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num += 1;
					}
					else {
						;	// DO NOTHING
					}

					if (type == D_BF_DCF_EXIF_SHORT_T) {
						memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.ValueOffset.SValue.Value, addr, count);
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.ValueOffset.SValue.dummy = 0;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Type = D_BF_DCF_EXIF_SHORT_T;
					}
					else {
						memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.ValueOffset.LValue, addr, count);
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelXDimension.Type = D_BF_DCF_EXIF_LONG_T;
					}
				}
				break;

			case FJ_EXIF_TAG_PEXELYDIM_:	// PixelYDimension
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Tag = FJ_EXIF_TAG_PEXELYDIM_;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num += 1;
					}
					else {
						;	// DO NOTHING
					}

					if (type == D_BF_DCF_EXIF_SHORT_T) {
						memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.ValueOffset.SValue.Value, addr, count);
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.ValueOffset.SValue.dummy = 0;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Type = D_BF_DCF_EXIF_SHORT_T;
					}
					else {
						memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.ValueOffset.LValue, addr, count);
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.PixelYDimension.Type = D_BF_DCF_EXIF_LONG_T;
					}
				}
				break;

			case FJ_EXIF_TAG_IMAGEUID_:	// ImageUID
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd.ImageUID.Tag = FJ_EXIF_TAG_IMAGEUID_;
						g_bf_dcf_exif_mpf_screen_app1[mpf_index].exif_ifd_num += 1;
					}
					else {
						;	// DO NOTHING
					}

					memcpy((FW_VOID*)g_bf_dcf_exif_mpf_screen_app1[mpf_index].image_uid_value, addr, count);
				}
				break;
			default:
				// DO NOTHING
				break;
		}
	}
	else {
		return -1;
	}

	return 0;
}
#else
FW_VOID BF_Dcf_Exif_Make_Exif_Mpg(T_BF_DCF_IF_SCREEN_DATA* scr_data)
{
	g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelXDimension.ValueOffset.LValue = scr_data->screen_width;
	g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelYDimension.ValueOffset.LValue = scr_data->screen_lines;

	if ((scr_data->thumb_addr != 0) && (scr_data->thumb_size != 0)) {
		g_bf_dcf_exif_mpf_screen_app1.ifd1_num                                            = D_BF_DCF_EXIF_MPF_IFD1_NUM;
		g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormat.Tag                      = FJ_IFD1_TAG_JPG_INTERCHGFMT_;
		g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormat.ValueOffset.LValue       = D_BF_DCF_EXIF_MPF_THUMB_OFFSET;
		g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormatLength.Tag                = FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_;
		g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormatLength.ValueOffset.LValue = scr_data->thumb_size;
	}
}

FW_VOID BF_Dcf_Exif_Copy_Exif_Mpf(FW_CHAR class, FW_CHAR* tmp_buff, FW_ULONG* mpf_head_length)
{
	FW_ULONG length = 0;

	if (tmp_buff == NULL) {
		return;
	}
	if (mpf_head_length == NULL) {
		return;
	}

	dcf_exif_set_offset_mpf();

	// Header Set
	memcpy(tmp_buff, (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.soi, D_BF_DCF_EXIF_SHORT_S);
	length += D_BF_DCF_EXIF_SHORT_S;
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.app1, D_BF_DCF_EXIF_APP1_MARKER_S);
	length += D_BF_DCF_EXIF_APP1_MARKER_S;
	memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.tiff_head, D_BF_DCF_EXIF_TIFF_HEADER_S);
	length += D_BF_DCF_EXIF_TIFF_HEADER_S;

	// IFD0 Set
	if (g_bf_dcf_exif_mpf_screen_app1.ifd0_num != 0) {
		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd0_num, D_BF_DCF_EXIF_SHORT_S);
		length += D_BF_DCF_EXIF_SHORT_S;

		if (g_bf_dcf_exif_mpf_screen_app1.ifd0.XResolution.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd0.XResolution, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}
		if (g_bf_dcf_exif_mpf_screen_app1.ifd0.YResolution.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd0.YResolution, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}
		if (g_bf_dcf_exif_mpf_screen_app1.ifd0.ResolutionUnit.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd0.ResolutionUnit, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}
		if (g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}

		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd0.offset_to_1stIFD, sizeof(FW_ULONG));
		length += sizeof(FW_ULONG);

		// Set IFD0 Value
		if (g_bf_dcf_exif_mpf_screen_app1.ifd0.XResolution.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd0_value.ValueXResolution, D_BF_DCF_EXIF_RATIONAL_S);
			length += D_BF_DCF_EXIF_RATIONAL_S;
		}
		if (g_bf_dcf_exif_mpf_screen_app1.ifd0.YResolution.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd0_value.ValueYResolution, D_BF_DCF_EXIF_RATIONAL_S);
			length += D_BF_DCF_EXIF_RATIONAL_S;
		}
	}

	// Exif IFD Set
	if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num != 0) {
		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num, D_BF_DCF_EXIF_SHORT_S);
		length += D_BF_DCF_EXIF_SHORT_S;

		if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}
		if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelXDimension.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelXDimension, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}
		if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelYDimension.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelYDimension, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}
		if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.ImageUID.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.exif_ifd.ImageUID, D_BF_DCF_EXIF_T_IFD_SIZE);
			length += D_BF_DCF_EXIF_T_IFD_SIZE;
		}

		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.exif_ifd.NextIFDOffset, sizeof(FW_ULONG));
		length += sizeof(FW_ULONG);

		// Set Exif IFD Value
		if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Tag != 0) {
			if (class == 1) {
				if (1 != FJ_Host_CreateMakerNote_Class1_CB((CHAR*)(tmp_buff + length), g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Count, length, NULL, 0)) {
					// Fail
					memset((tmp_buff + length), 0, g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Count);
				}
			}
			else {
				if (1 != FJ_Host_CreateMakerNote_Class2_CB((CHAR*)(tmp_buff + length), g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Count, length, NULL, 0)) {
					// Fail
					memset((tmp_buff + length), 0, g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Count);
				}
			}
			length += g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Count;
		}
		if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.ImageUID.Tag != 0) {
			memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.image_uid_value, D_BF_DCF_EXIF_IMAGEUID_SIZE);
			length += D_BF_DCF_EXIF_IMAGEUID_SIZE;
		}
	}

	// IFD1 Set
	if (g_bf_dcf_exif_mpf_screen_app1.ifd1_num != 0) {
		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd1_num, D_BF_DCF_EXIF_SHORT_S);
		length += D_BF_DCF_EXIF_SHORT_S;

		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormat, D_BF_DCF_EXIF_T_IFD_SIZE);
		length += D_BF_DCF_EXIF_T_IFD_SIZE;
		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormatLength, D_BF_DCF_EXIF_T_IFD_SIZE);
		length += D_BF_DCF_EXIF_T_IFD_SIZE;

		memcpy((tmp_buff + length), (FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd1.NextIFDOffset, sizeof(FW_ULONG));
		length += sizeof(FW_ULONG);
	}

	*mpf_head_length = length;
	BF_Dcf_Exif_Reset_App1_Mpf();
}



FW_VOID BF_Dcf_Exif_Reset_App1_Mpf(FW_VOID)
{
	g_bf_dcf_exif_mpf_screen_app1.ifd0_num                                 = D_BF_DCF_EXIF_MPF_IFD0_NUM;

	// IFD0 Tag Reset
	g_bf_dcf_exif_mpf_screen_app1.ifd0.XResolution.Tag                     = 0x0000;
	g_bf_dcf_exif_mpf_screen_app1.ifd0.YResolution.Tag                     = 0x0000;
	g_bf_dcf_exif_mpf_screen_app1.ifd0.ResolutionUnit.Tag                  = 0x0000;
	g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer.Tag                  = FJ_IFD0_TAG_EXIFPTR_;

	g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer.ValueOffset.Offset   = 0x00000000;
	g_bf_dcf_exif_mpf_screen_app1.ifd0.offset_to_1stIFD                    = 0x00000000;

	g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num                             = D_BF_DCF_EXIF_MPF_EXIFIFD_NUM;

	// Exif IFD Tag Reset
	g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Tag                   = 0x0000;
	g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelXDimension.Tag             = FJ_EXIF_TAG_PEXELXDIM_;
	g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelYDimension.Tag             = FJ_EXIF_TAG_PEXELYDIM_;
	g_bf_dcf_exif_mpf_screen_app1.exif_ifd.ImageUID.Tag                    = 0x0000;

	g_bf_dcf_exif_mpf_screen_app1.ifd1_num                                 = 0;

	// IFD1 Tag Reset
	g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormat.Tag           = 0x0000;
	g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormatLength.Tag     = 0x0000;
}

static FW_VOID dcf_exif_set_offset_mpf(FW_VOID)
{
	if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num == 0) {
		g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer.Tag = 0x0000;
		g_bf_dcf_exif_mpf_screen_app1.ifd0_num -= 1;
	}

	g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer.ValueOffset.Offset = (D_BF_DCF_EXIF_TIFFHEADSIZE + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * g_bf_dcf_exif_mpf_screen_app1.ifd0_num) + 4);
	if (g_bf_dcf_exif_mpf_screen_app1.ifd1_num != 0) {
		g_bf_dcf_exif_mpf_screen_app1.ifd0.offset_to_1stIFD                         = (g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer.ValueOffset.Offset + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num) + 4);
		g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormat.ValueOffset.LValue = (g_bf_dcf_exif_mpf_screen_app1.ifd0.offset_to_1stIFD                  + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * g_bf_dcf_exif_mpf_screen_app1.ifd1_num)     + 4);
	}
	else {
		g_bf_dcf_exif_mpf_screen_app1.ifd0.offset_to_1stIFD                         = 0;
		g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormat.ValueOffset.LValue = 0;
	}

	if (g_bf_dcf_exif_mpf_screen_app1.ifd0.XResolution.Tag != 0) {
		g_bf_dcf_exif_mpf_screen_app1.ifd0.XResolution.ValueOffset.Offset = g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer.ValueOffset.Offset;

		g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer.ValueOffset.Offset += sizeof(T_BF_DCF_EXIF_RATIONAL);
		if (g_bf_dcf_exif_mpf_screen_app1.ifd1_num != 0) {
			g_bf_dcf_exif_mpf_screen_app1.ifd0.offset_to_1stIFD                         += sizeof(T_BF_DCF_EXIF_RATIONAL);
			g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormat.ValueOffset.LValue += sizeof(T_BF_DCF_EXIF_RATIONAL);
		}
	}

	if (g_bf_dcf_exif_mpf_screen_app1.ifd0.YResolution.Tag != 0) {
		g_bf_dcf_exif_mpf_screen_app1.ifd0.YResolution.ValueOffset.Offset = g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer.ValueOffset.Offset;

		g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer.ValueOffset.Offset += sizeof(T_BF_DCF_EXIF_RATIONAL);
		if (g_bf_dcf_exif_mpf_screen_app1.ifd1_num != 0) {
			g_bf_dcf_exif_mpf_screen_app1.ifd0.offset_to_1stIFD                         += sizeof(T_BF_DCF_EXIF_RATIONAL);
			g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormat.ValueOffset.LValue += sizeof(T_BF_DCF_EXIF_RATIONAL);
		}
	}

	if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Tag != 0) {
		g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.ValueOffset.Offset = (g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer.ValueOffset.Offset + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num) + 4);

		if (g_bf_dcf_exif_mpf_screen_app1.ifd1_num != 0) {
			g_bf_dcf_exif_mpf_screen_app1.ifd0.offset_to_1stIFD                         += g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Count;
			g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormat.ValueOffset.LValue += g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Count;
		}
	}

	if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.ImageUID.Tag != 0) {
		g_bf_dcf_exif_mpf_screen_app1.exif_ifd.ImageUID.ValueOffset.Offset = (g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer.ValueOffset.Offset + 2 + (sizeof(T_BF_DCF_EXIF_IFD) * g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num) + 4);
		if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Tag != 0) {
			g_bf_dcf_exif_mpf_screen_app1.exif_ifd.ImageUID.ValueOffset.Offset += g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Count;
		}

		if (g_bf_dcf_exif_mpf_screen_app1.ifd1_num != 0) {
			g_bf_dcf_exif_mpf_screen_app1.ifd0.offset_to_1stIFD                         += g_bf_dcf_exif_mpf_screen_app1.exif_ifd.ImageUID.Count;
			g_bf_dcf_exif_mpf_screen_app1.ifd1.JPEGInterchangeFormat.ValueOffset.LValue += g_bf_dcf_exif_mpf_screen_app1.exif_ifd.ImageUID.Count;
		}
	}

#if 0
	if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num == 0) {
		g_bf_dcf_exif_mpf_screen_app1.ifd0.ExifIFDPointer.ValueOffset.Offset = 0x00000000;
	}
#endif
}


FW_INT32 BF_Dcf_Exif_Set_Value_Mpf(FW_USHORT ifd, FW_USHORT tag, FW_USHORT type, FW_USHORT count, FW_UCHAR* addr)
{
	switch (type) {
		case 0xFFFF:		// FALL THROUGH
		case D_BF_DCF_EXIF_BYTE_T:		// FALL THROUGH
		case D_BF_DCF_EXIF_ASCII_T:		// FALL THROUGH
		case D_BF_DCF_EXIF_UNDEFINED_T:
			break;

		case D_BF_DCF_EXIF_SHORT_T:
			count *= D_BF_DCF_EXIF_SHORT_S;
			break;

		case D_BF_DCF_EXIF_LONG_T:
			count *= D_BF_DCF_EXIF_LONG_S;
			break;

		case D_BF_DCF_EXIF_RATIONAL_T:	// FALL THROUGH
		case D_BF_DCF_EXIF_SRATIONAL_T:
			count *= D_BF_DCF_EXIF_RATIONAL_S;
			break;

		default:
			return -1;
	}

	if (ifd == FJ_IFD_0) {
		switch (tag) {
			case FJ_IFD0_TAG_XRES_:		// XResolution
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1.ifd0.XResolution.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1.ifd0.XResolution.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1.ifd0_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1.ifd0.XResolution.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1.ifd0.XResolution.Tag = FJ_IFD0_TAG_XRES_;
						g_bf_dcf_exif_mpf_screen_app1.ifd0_num += 1;
					}
					else {
						;	// DO NOTHING
					}

					memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd0_value.ValueXResolution, addr, count);
				}
				break;

			case FJ_IFD0_TAG_YRES_:		// YResolution
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1.ifd0.YResolution.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1.ifd0.YResolution.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1.ifd0_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1.ifd0.YResolution.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1.ifd0.YResolution.Tag = FJ_IFD0_TAG_YRES_;
						g_bf_dcf_exif_mpf_screen_app1.ifd0_num += 1;
					}
					else {
						;	// DO NOTHING
					}

					memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd0_value.ValueYResolution, addr, count);
				}
				break;

			case FJ_IFD0_TAG_RESUNIT_:		// ResolutionUnit
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1.ifd0.ResolutionUnit.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1.ifd0.ResolutionUnit.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1.ifd0_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1.ifd0.ResolutionUnit.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1.ifd0.ResolutionUnit.Tag = FJ_IFD0_TAG_RESUNIT_;
						g_bf_dcf_exif_mpf_screen_app1.ifd0_num += 1;
					}
					else {
						;	/// DO NOTHING
					}

					memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.ifd0.ResolutionUnit.ValueOffset.SValue.Value, addr, count);
				}
				break;

			default:
				// Error
				return -1;
		}
	}
	else if (ifd == FJ_IFD_EXIF) {
		switch (tag) {
			case FJ_EXIF_TAG_MAKERNOTE_:	// MakerNote
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Tag = FJ_EXIF_TAG_MAKERNOTE_;
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num += 1;
					}
					else {
						;	// DO NOTHING
					}

					g_bf_dcf_exif_mpf_screen_app1.exif_ifd.MakerNote.Count = count;
				}
				break;

			case FJ_EXIF_TAG_PEXELXDIM_:	// PixelXDimension
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelXDimension.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelXDimension.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelXDimension.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelXDimension.Tag = FJ_EXIF_TAG_PEXELXDIM_;
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num += 1;
					}
					else {
						;	// DO NOTHING
					}

					if (type == D_BF_DCF_EXIF_SHORT_T) {
						memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelXDimension.ValueOffset.SValue.Value, addr, count);
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelXDimension.ValueOffset.SValue.dummy = 0;
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelXDimension.Type = D_BF_DCF_EXIF_SHORT_T;
					}
					else {
						memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelXDimension.ValueOffset.LValue, addr, count);
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelXDimension.Type = D_BF_DCF_EXIF_LONG_T;
					}
				}
				break;

			case FJ_EXIF_TAG_PEXELYDIM_:	// PixelYDimension
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelYDimension.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelYDimension.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelYDimension.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelYDimension.Tag = FJ_EXIF_TAG_PEXELYDIM_;
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num += 1;
					}
					else {
						;	// DO NOTHING
					}

					if (type == D_BF_DCF_EXIF_SHORT_T) {
						memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelYDimension.ValueOffset.SValue.Value, addr, count);
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelYDimension.ValueOffset.SValue.dummy = 0;
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelYDimension.Type = D_BF_DCF_EXIF_SHORT_T;
					}
					else {
						memcpy((FW_VOID*)&g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelYDimension.ValueOffset.LValue, addr, count);
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.PixelYDimension.Type = D_BF_DCF_EXIF_LONG_T;
					}
				}
				break;

			case FJ_EXIF_TAG_IMAGEUID_:	// ImageUID
				if (type == 0xFFFF) {
					// Tag Delete
					if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.ImageUID.Tag != 0) {
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.ImageUID.Tag = 0;
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num -= 1;
					}
					else {
						;	// DO NOTHING
					}
				}
				else {
					if (g_bf_dcf_exif_mpf_screen_app1.exif_ifd.ImageUID.Tag == 0) {
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd.ImageUID.Tag = FJ_EXIF_TAG_IMAGEUID_;
						g_bf_dcf_exif_mpf_screen_app1.exif_ifd_num += 1;
					}
					else {
						;	// DO NOTHING
					}

					memcpy((FW_VOID*)g_bf_dcf_exif_mpf_screen_app1.image_uid_value, addr, count);
				}
				break;
			default:
				// DO NOTHING
				break;
		}
	}
	else {
		return -1;
	}

	return 0;
}
#endif
#endif	// CO_BF_DCF_IF_MPF_ENABLE

/**
 * @brief analyze exif tag offset in JPEG.
 * @param i:file_id file id
 * @param o:*load_image_data image data info
 * @param o:*img_detail image detail information
 * @return error code
 * @note none
 * @attention none.
 */
FW_INT32 BF_Dcf_Exif_Analyze_Jpeg_Date(FW_INT32 file_id, T_BF_DCF_IF_DATE* datetime_buf)
{
	FW_INT32  ercd, ercd2;
	FW_UCHAR  *buffer;
	FW_ULONG  result;
	FW_ULONG  tag_offset = 0;
	FW_ULONG  reset_offset = 0;
	FW_ULONG  value_offset = 0;
	FW_ULONG  total_buffer = D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE;
	FW_UCHAR  tag_type;
	FW_ULONG  tag_count;
	FW_ULONG  data_offset;
	FW_USHORT app_zero_size = 0;
	FW_USHORT comment_size = 0;
	FW_USHORT jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET;
	FW_USHORT zero_ifd_offset   = D_BF_DCF_EXIF_TIFFHEADSIZE;

	if (datetime_buf == NULL) {
		// parameter error
		return FSDCF_ERRNO_TOUCH_NULL_POINTER;
	}

	// data clear
#ifdef FJ_CUSTOM_EXIF_BIG_ENDIAN
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_BIG_EN;
#else
	g_bf_dcf_exif_big_or_lit = D_BF_DCF_EXIF_LIT_EN;
#endif

	// memory allocation
	ercd = BF_Fs_User_Custom_Get_Blf_Exif_Buf((FW_VOID**)&buffer);
	if (ercd != D_BF_FS_USER_CUSTOM_OK) {
		// memory get error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Get_Blf_Exif_Buf() failed: ercd=%d\n", ercd));
#endif
		return ercd;
	}

	while (1) {
		// read data from Exif
		// file pointer : file top
		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
			// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		// check DQT marker
		if (*(buffer + 3) == 0xDB) {
			// detect JPEG(without APPx) format
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;

		}
		// check APP0 marker
		else if (*(buffer + 3) == 0xE0) {
			// detect APP0
			app_zero_size = dcf_exif_get_short_value_big_endian(buffer + 4) + 2;
			jpeg_start_offset = D_BF_DCF_EXIF_TIFFOFFSET + app_zero_size;

			// check COM marker
			if (*(buffer + app_zero_size + 3) == 0xFE) {
				comment_size = dcf_exif_get_short_value_big_endian(buffer + app_zero_size + 4) + 2;
				app_zero_size = app_zero_size + comment_size;
			}
		}
		else {
			; // DO NOTHING
		}

		// check DQT marker
		if (*(buffer + app_zero_size + 3) == 0xDB) {
			// detect JFIF(no thumbnail) format
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;

		}
		// check APP0 marker
		else if (*(buffer + app_zero_size + 3) == 0xE0) {
			// detect JFIF(JFXX) format
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;

		}
		// check APP1 marker
		// APP1 marker's place is next to SOI marker
		else if (*(buffer + app_zero_size + 3) != 0xE1) {
			// not detect APP1
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND APP1 MARKER\n"));
#endif
			ercd = FSDCF_ERRNO_NOT_APP1;
			break;
		}
		else {
			; // DO NOTHING
		}

		// reset pointer
		ercd = BF_Mdf_Wrapper_Lseek(file_id, 0, SEEK_SET, &result);
		if (ercd != 0) {
			// lseek error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Lseek() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		ercd = BF_Mdf_Wrapper_Read(file_id, buffer, D_BF_FS_USER_CUSTOM_MPFID_EXIF_BUF_SIZE, &result);
		if (ercd != 0) {
			// read error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("BF_Mdf_Wrapper_Read() failed: ercd=%d, file_id=%x\n", ercd, file_id));
#endif
			break;
		}

		// check endian
		g_bf_dcf_exif_big_or_lit = *((FW_USHORT*)(buffer + jpeg_start_offset));

		zero_ifd_offset  = dcf_exif_get_long_value(buffer + jpeg_start_offset + 4);

		tag_offset   = jpeg_start_offset + zero_ifd_offset + 2;
		reset_offset = jpeg_start_offset + zero_ifd_offset + 2;
		gNumOfTag = dcf_exif_get_short_value(buffer + tag_offset - 2);	// Num of 0th IFD's tag
		gTagCheckCnt = 1;	// reset seearch count

		// Get EXIF IDF offset
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_IFD0_TAG_EXIFPTR_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND EXIF IDF OFFSET\n"));
#endif
			break;
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
		}
		data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);

		// get next start offset
		ercd = dcf_exif_get_exif_idf_offset(file_id, buffer, data_offset, &tag_offset, &reset_offset, &total_buffer);
		if (ercd != 0) {
			// error
			break;
		}

		// Get date & time Original
		ercd = dcf_exif_check_exif_tag(file_id, buffer, FJ_EXIF_TAG_DATETIMEORIG_, &tag_offset, &reset_offset, &value_offset, &total_buffer, &tag_type, &tag_count);
		if (ercd == FSDCF_ERRNO_EXIF_TAG_OVER) {
			// tag not find
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
			BF_Fs_User_Custom_Err_Printer(("CAN NOT FIND DATE TIME ORIGINAL\n"));
#endif
			break;
		}
		else if (ercd != 0) {
			// error
			break;
		}
		else {
			// get date & time Original
			data_offset = jpeg_start_offset + dcf_exif_get_long_value(buffer + tag_offset + value_offset);
			ercd = dcf_exif_get_exif_date_value(file_id, buffer, &data_offset, total_buffer, datetime_buf);
			if (ercd != 0) {
				// get value error
				break;
			}
		}
		break;
	}

	ercd2 = BF_Fs_User_Custom_Rel_Blf_Exif_Buf((FW_VOID*)buffer);
	if (ercd2 != D_BF_FS_USER_CUSTOM_OK) {
		// memory release error
#ifdef CO_BF_FS_USER_CUSTOM_DCFE_ERR_PRINT
		BF_Fs_User_Custom_Err_Printer(("BF_Fs_User_Custom_Rel_Blf_Exif_Buf() failed: ercd=%d\n", ercd2));
#endif
		ercd = ercd2;
	}

	return ercd;
}
