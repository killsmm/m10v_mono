/**
 * @file		dcf_if.h
 * @brief		User interface of FileSystem function with depend on DCF(header).
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _DCF_IF_H_
#define _DCF_IF_H_

#include "fs_if.h"
#include "fj_custom.h"
#include "fj_dcf.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
// for MPF -------------------------------------------
#define CO_BF_DCF_IF_MPF_ENABLE
//#define CO_BF_DCF_IF_JPG_SAVE_PADD_ON
#define CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

#define CO_BF_DCF_IF_ADD_EXIF_INFO
#define CO_BF_DCF_IF_GET_GPSTAG
#define CO_BF_DCF_IF_SET_GPSTAG
// for Jpeg-HDR --------------------------------------
#define CO_BF_DCF_IF_APP11_ENABLE

/* for name of fullpass and object, extenstion */
#define D_BF_DCF_IF_FULLPASS_FILENAME_MAX		(766)
#define D_BF_DCF_IF_OBJ_NAME_MAX				(19)
#define D_BF_DCF_IF_EXT_MAX						(4)

/* for DCF DB */
#define D_BF_DCF_IF_TOP_DIR_NAME_MAX			(D_BF_FS_IF_DRVNAME_MAX+7)	// "[DriveName=8]:\DCIM\"
#define D_BF_DCF_IF_DIR_NAME_MAX				(9)
#define D_BF_DCF_IF_OBJ_ONLY_NAME_MAX			(9)
#define D_BF_DCF_IF_DIR_NAME_ONLY				(6)	// 5 charactor + NULL
#define D_BF_DCF_IF_FILE_NAME_ONLY				(5)	// 4 charactor + NULL
#define D_BF_DCF_IF_FILE_PATH_WO_DRVNAME		(28)	// DCF File path size without drive name length.  :\DCIM\100AAAAA\BBBB0001.JPG

/* action mode for BF_Dcf_If_Set_Current_Dir() */
#define D_BF_DCF_IF_PRM_NORMAL_ACTION			(1)
//#define D_BF_DCF_IF_PRM_CURRENT_ONLY			(2)
//#define D_BF_DCF_IF_PRM_PREV_NEXT_ONLY		(4)

#define D_BF_DCF_IF_ROOT_DIR_NAME				"DCIM"

#define D_BF_DCF_IF_FILE_TYPE_JPG_MAIN			(0x00)		// This type means a main image in the JPG file.
#define D_BF_DCF_IF_FILE_TYPE_JPG_SCRNAIL		(0x01)		// This type means a screen nail image(640x480) in the JPG file.
#define D_BF_DCF_IF_FILE_TYPE_JPG_THM			(0x02)		// This type means a thumbnail image(160x120) in the JPG file.
#define D_BF_DCF_IF_FILE_TYPE_VIDEO_MAIN		(0x10)		// This type means a main video file.             "TECB0001.MP4"
#define D_BF_DCF_IF_FILE_TYPE_VIDEO_SUB			(0x11)		// This type means a sub video file.              "TECS0001.MP4"
#define D_BF_DCF_IF_FILE_TYPE_VIDEO_THM			(0x12)		// This type means a thumbnail file of video fil. "TECB0001.THM"
#define D_BF_DCF_IF_FILE_TYPE_VIDEO_TOP			(0x13)		// This type means top image of " TECB0001.MP4"

typedef FW_VOID (*DCF_ENTRY)(FW_INT32 ret);
/******************************************************************************
 Global Function

 <sample>
  "entry_func" implement is as follows:
	FW_VOID Sample_Func(FW_INT32 result)
	{
		;
	}

******************************************************************************/

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/**
 * @breif exposure progrem for DCF
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_EXPOSURE_NOT_DEFINED = 0,
	E_BF_DCF_IF_EXPOSURE_MANUAL = 1,
	E_BF_DCF_IF_EXPOSURE_NORMAL = 2,
	E_BF_DCF_IF_EXPOSURE_APERTURE = 3,
	E_BF_DCF_IF_EXPOSURE_SHUTTER = 4,
	E_BF_DCF_IF_EXPOSURE_CREATE = 5,
	E_BF_DCF_IF_EXPOSURE_ACTION = 6,
	E_BF_DCF_IF_EXPOSURE_PORTRAIT = 7,
	E_BF_DCF_IF_EXPOSURE_LANDSCAPE = 8
} E_BF_DCF_IF_EXPOSURE;

/**
 * @breif metering mode for DCF
 * @note none
 * @attention the following values are exif tag
 */
typedef enum {
	E_BF_DCF_IF_METERING_UNKNOWN = 0,			/**< unknown */
	E_BF_DCF_IF_METERING_AVERAGE = 1,			/**< average */
	E_BF_DCF_IF_METERING_CENTER_WEIGHTED = 2,	/**< center weighted average */
	E_BF_DCF_IF_METERING_SPOT = 3,			/**< spot */
	E_BF_DCF_IF_METERING_MULTI_SPOT = 4,		/**< multi spot */
	E_BF_DCF_IF_METERING_MULTI = 5,			/**< pattern */
	E_BF_DCF_IF_METERING_PATTERN = 5,			/**< pattern */
	E_BF_DCF_IF_METERING_PARTIAL = 6,			/**< partial */
	E_BF_DCF_IF_METERING_OTHER = 255			/**< other */
} E_BF_DCF_IF_METERING;

/**
 * white balance
 */
/**
 * @breif white balance mode
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_WB_MANUAL = 0,       /**< manual */
	E_BF_DCF_IF_WB_AUTO = 1,         /**< auto */
	E_BF_DCF_IF_WB_DAYLIGHT = 2,     /**< daylight */
	E_BF_DCF_IF_WB_CLOUDY = 3,       /**< cloudy */
	E_BF_DCF_IF_WB_FLOURESCENT = 4,  /**< flourescent */
	E_BF_DCF_IF_WB_TUNGSTEN = 5      /**< tungsten */
} E_BF_DCF_IF_WB;

/**
 * @breif scene capture type for DCF
 * @note none
 * @attention none
 */
typedef enum {
    E_BF_DCF_IF_SCENE_STANDARD = 0,
    E_BF_DCF_IF_SCENE_LANDSCAPE = 1,
    E_BF_DCF_IF_SCENE_PORTRAIT = 2,
    E_BF_DCF_IF_SCENE_NIGHT = 3
} E_BF_DCF_IF_SCENE;

/**
 * @breif file extension type for DCF
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_FILE_TYPE_NOTHING = 0x00,
	E_BF_DCF_IF_FILE_TYPE_JPG = 0x01,
//#if ( CO_MOVIE_FF_AUDIO_PATTERN == 0 )
	E_BF_DCF_IF_FILE_TYPE_MP4 = 0x02,
//#else
	E_BF_DCF_IF_FILE_TYPE_MOV = 0x04,
//#endif
	E_BF_DCF_IF_FILE_TYPE_RAW = 0x08,
	E_BF_DCF_IF_FILE_TYPE_WAV = 0x10,
	E_BF_DCF_IF_FILE_TYPE_THM = 0x20,
	E_BF_DCF_IF_FILE_TYPE_AVI = 0x40,
	E_BF_DCF_IF_FILE_TYPE_TIF = 0x80
} E_BF_DCF_IF_FILE_TYPE;

/**
 * @breif file index type for DCF
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_FILE_INDEX_TYPE_PHOTO = 0x01, // jpeg,tiff
	E_BF_DCF_IF_FILE_INDEX_TYPE_MOVIE = 0x02 // mov
} E_BF_DCF_IF_FILE_INDEX_TYPE;

/**
 * @breif file attrubute for DCF
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_ATTR_NORMAL = 0x00,
	E_BF_DCF_IF_ATTR_RDONLY = 0x01
} E_BF_DCF_IF_ATTR;

/**
 * @breif file duplication status for DCF
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_UNDUPLICATE = 0x00,
	E_BF_DCF_IF_DUPLICATE = 0x01
} E_BF_DCF_IF_DUPLICATION_STATUS;

/**
 * @breif voice memo exist flag for DCF
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_OPTION_FILE_NOTHING = 0x00,
	E_BF_DCF_IF_OPTION_FILE_EXIST = 0x01
} E_BF_DCF_IF_OPTION_FILE;

/**
 * @breif image orientation for DCF
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_ROTATE_0			 = 0x01,
	E_BF_DCF_IF_ROTATE_MIRROR_0		 = 0x02,
	E_BF_DCF_IF_ROTATE_180			 = 0x03,
	E_BF_DCF_IF_ROTATE_MIRROR_180	 = 0x04,
	E_BF_DCF_IF_ROTATE_MIRROR_90	 = 0x05,
	E_BF_DCF_IF_ROTATE_90			 = 0x06,
	E_BF_DCF_IF_ROTATE_MIRROR_270	 = 0x07,
	E_BF_DCF_IF_ROTATE_270			 = 0x08
} E_BF_DCF_IF_ROTATE;

/**
 * @breif DPOF setting information
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_DPOF_SET_OFF = 0x00,
	E_BF_DCF_IF_DPOF_SET_ON = 0x01
} E_BF_DCF_IF_DPOF_SET;

/**
 * @breif DPOF index setting information
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_DPOF_INDEX_NO = 0x00,
	E_BF_DCF_IF_DPOF_INDEX_YES = 0x01
} E_BF_DCF_IF_DPOF_INDEX;

/**
 * @breif DPOF size setting information
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_DPOF_SIZE_NO = 0x00,
	E_BF_DCF_IF_DPOF_SIZE_3X5 = 0x03,
	E_BF_DCF_IF_DPOF_SIZE_4X6 = 0x04,
	E_BF_DCF_IF_DPOF_SIZE_5X7 = 0x05,
	E_BF_DCF_IF_DPOF_SIZE_8X10 = 0x08
} E_BF_DCF_IF_DPOF_SIZE;

/**
 * @breif jpeg file format type
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_JPEG_FORMAT_EXIF = 0,
	E_BF_DCF_IF_JPEG_FORMAT_NOT_EXIF
} E_BF_DCF_IF_JPEG_FORMAT;

/**
 * @breif flag of execute remove, when file close
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_REMOVE_FLG_OFF = 0x00,
	E_BF_DCF_IF_REMOVE_FLG_ON = 0x01
} E_BF_DCF_IF_REMOVE_FLG;

/**
 * @breif File name setting
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_FILE_NAME_RESET = 0x00,
	E_BF_DCF_IF_FILE_NAME_SERIES = 0x01
} E_BF_DCF_IF_FILE_NAME;

/**
 * @breif qt type
 * @note none
 * @attention none
 */
typedef enum {
	E_BF_DCF_IF_QT_TYPE_MAIN = 0x00,
	E_BF_DCF_IF_QT_TYPE_THUMB = 0x01,
	E_BF_DCF_IF_QT_TYPE_VGA = 0x02
} E_BF_DCF_IF_QT_TYPE;

/**
 * @brief latest type.
 * @note none
 * @attention none.
 */
typedef enum {
	E_BF_DCF_IF_LATEST_TYPE_ALL = 0x00,
	E_BF_DCF_IF_LATEST_TYPE_MEDIA = 0x01,
	E_BF_DCF_IF_LATEST_TYPE_SPECIFIED = 0x02,
	E_BF_DCF_IF_LATEST_TYPE_SELECT = 0x03
} E_BF_DCF_IF_LATEST_TYPE;

/**
 * @brief DCF file create option.
 * @note none
 * @attention none.
 */
typedef enum {
	E_BF_DCF_IF_CREATE_UPDATE = 0x00,
	E_BF_DCF_IF_CREATE_NEW_FILE = 0x01
} E_BF_DCF_IF_CREATE;

/**
 * @brief Target that adds file
 * @note none
 * @attention none.
 */
typedef enum {
	E_BF_DCF_IF_UPDATE_DB = 0,			// update file in DCF DB and Date DB
	E_BF_DCF_IF_UPDATE_DATE_DB_COPY,	// update file in Date DB (for file operation copy)
	E_BF_DCF_IF_UPDATE_DATE_VOICE_MEMO	// update file in Date DB (for voice memo)
} E_BF_DCF_IF_UPDATE_KIND;

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @brief MPF Extended MP Index
 * @note none
 * @attention none.
 */
typedef enum {
	E_BF_DCF_IF_MPF_EXT_MP_SELECT_1= 0,	
	E_BF_DCF_IF_MPF_EXT_MP_SELECT_2,
	E_BF_DCF_IF_MPF_EXT_MP_SELECT_MAX
} E_BF_DCF_IF_MPF_EXT_MP_SELECT;
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

/**
 * @brief APP11 Marker Existence
 * @note none
 * @attention none.
 */
typedef enum {
	E_BF_DCF_IF_APP11_FLG_OFF = 0x00,
	E_BF_DCF_IF_APP11_FLG_ON  = 0x01
} E_BF_DCF_IF_APP11_FLG;

/**
 * @brief Data Type
 * @note none
 * @attention none.
 */
typedef enum {
	E_BF_DCF_IF_DATA_TYPE_RAW = 0x00,
	E_BF_DCF_IF_DATA_TYPE_DNG = 0x01
} E_BF_DCF_IF_DATA_TYPE;

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
/**
 * @breif file index structure for DCF
 * @note none
 * @attention none
 */
/* file index */
typedef union {
	FW_LONG index;
	struct {
#ifdef FJ_CUSTOM_CPU_BIG_ENDIAN
		FW_SHORT dir_index;
		FW_SHORT obj_index;
#else
		FW_SHORT obj_index;
		FW_SHORT dir_index;
#endif
	} pack;
} T_BF_DCF_IF_FILE_INDEX;

/**
 * @breif object structure for DCF
 * @note none
 * @attention none
 */
/* object information */
typedef struct {
  FW_UCHAR extType;     // extension type. bit field.
  FW_UCHAR attr;         // file attribute
} T_BF_DCF_IF_DCFDB_OBJ_INFO;


/**
 * @breif date structure for DCF
 * @note none
 * @attention none
 */
typedef struct {
	FW_USHORT ad_year;			/* date A.D. year */
	FW_UCHAR month;				/* date month */
	FW_UCHAR day;				/* date day */
	FW_UCHAR hour;				/* date hour */
	FW_UCHAR min;				/* date minute */
	FW_UCHAR sec;				/* date second */
} T_BF_DCF_IF_DATE;

/**
 * @breif fraction structure for DCF
 * @note none
 * @attention none
 */
typedef struct {
	FW_ULONG nume;				/**< numerator */
	FW_ULONG denomi;			/**< denominator */
} T_BF_DCF_IF_FRACTION;

/**
 * @breif fraction structure for DCF 
 * @note none
 * @attention none
 */
typedef struct {
	FW_LONG nume;				/**< numerator */
	FW_ULONG denomi;			/**< denominator */
} T_BF_DCF_IF_SFRACTION;

/**
 * @breif date structure for GPS
 * @note none
 * @attention none
 */
typedef struct {
	FW_UCHAR latitude_ref;
	T_BF_DCF_IF_FRACTION latitude[3];
	FW_UCHAR longitude_ref;
	T_BF_DCF_IF_FRACTION longitude[3];
	FW_UCHAR img_direction_ref;
	T_BF_DCF_IF_FRACTION img_direction;
} T_BF_DCF_IF_GPS;

/**
 * @breif saved exif info structure for DCF
 * @note none
 * @attention none
 */
typedef struct {
	FW_USHORT width;		// image width
	FW_USHORT lines;		// image length
	FW_DOUBLE ev;
	T_BF_DCF_IF_DATE  date;
	T_BF_DCF_IF_FRACTION exposure_time;
	T_BF_DCF_IF_FRACTION f_value;
	E_BF_DCF_IF_EXPOSURE exposure_prog;
	T_BF_DCF_IF_FRACTION shutter_speed;
	T_BF_DCF_IF_FRACTION aperture;
	T_BF_DCF_IF_FRACTION max_aperture;
	E_BF_DCF_IF_METERING metering;
	E_BF_DCF_IF_WB light_source;
	FW_USHORT flash;
	T_BF_DCF_IF_FRACTION dzoom_ratio;
	FW_USHORT iso_value;
	T_BF_DCF_IF_FRACTION focal_length;
	FW_SHORT sharpness;
	FW_USHORT maker_note_length;
	FW_USHORT qvga_enable;
	T_BF_DCF_IF_GPS  gps;
} T_BF_DCF_IF_EXIF_INFO;

/**
 * @breif DPOF information structure
 * @note none
 * @attention none
 */
typedef struct {
	E_BF_DCF_IF_DPOF_SET dpof_set;	/* print order check */
	FW_USHORT dpof_sheets;		/* sheet number */
	E_BF_DCF_IF_DPOF_INDEX dpof_index;/* index yes/no */
	E_BF_DCF_IF_DPOF_SIZE dpof_size;	/* print size */
} T_BF_DCF_IF_DPOF_INFO ;


/**
 * @breif file informaiotn structure
 * @note none
 * @attention none
 */
typedef struct {
	E_BF_MEDIA_STATE_MEDIA_TYPE media_type;	// stored media(MS,SD)
	FW_USHORT dir_no;				// file directory number(100-999)
	FW_USHORT file_no;				// file number(1-9999)
	FW_CHAR file_char[5];			// file charactor
	FW_LONG file_type;		// file type(jpeg,raw,wav,mpeg)
	E_BF_DCF_IF_ATTR file_attr;		// file attribute(normal,read-only)
	E_BF_DCF_IF_OPTION_FILE option_file;	// voice memo exist flag
	T_BF_DCF_IF_DPOF_INFO dpof_info;		// Dpof information
	E_BF_DCF_IF_JPEG_FORMAT jpeg_format;
	E_BF_DCF_IF_DUPLICATION_STATUS file_dupstatus;	// file number duplication status
} T_BF_DCF_IF_FILE_INFO;

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @breif individual image informaion structure
 * @note none
 * @attention none
 */
typedef struct {
	FW_ULONG main_width;
	FW_ULONG main_lines;
	FW_UCHAR   ycc_format;	// YCbCr format  0x21 : ycbcr422 , 0x22 : ycbcr420
	FW_UCHAR   ycc_format_thum;	// YCbCr format  0x21 : ycbcr422 , 0x22 : ycbcr420
	FW_ULONG class1_width;
	FW_ULONG class1_lines;
	FW_UCHAR class1_ycc_format;	// YCbCr format  0x21 : ycbcr422 , 0x22 : ycbcr420
	FW_ULONG class2_width; 
	FW_ULONG class2_lines;
	FW_UCHAR class2_ycc_format;	// YCbCr format  0x21 : ycbcr422 , 0x22 : ycbcr420
} T_BF_DCF_IF_EXT_MP_IMAGE_DETAIL;
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

/**
 * @breif file detail informaion structure
 * @note none
 * @attention none
 */
typedef struct {
	T_BF_DCF_IF_FILE_INFO file_info;
	E_BF_DCF_IF_ROTATE rotate;		// rotation direction
	E_BF_DCF_IF_ROTATE rotate_thumb;		// rotation direction
	FW_ULONG width;			// image width
	FW_ULONG lines;			// image length
	FW_USHORT iso;			// iso value
	FW_USHORT flash;			// flash on/off
	FW_ULONG EpTimeNum;			// Exposure Time Numerator(TV)
	FW_ULONG EpTimeDen;			// Exposure Time Denominator(TV)
	FW_ULONG FvNum;			// F Value Numerator(AV)
	FW_ULONG FvDen;			// F Value Denominator(AV)
	FW_SHORT EpProgram;			// Exposure Program
	FW_SHORT MeteringMode;		// MeteringMode
	FW_SHORT LightSource;		// LightSource
	FW_SHORT WBMode;			// WhiteBalanceMode
	T_BF_DCF_IF_DATE edit_date_time;		// edit date and time(case of rotate and resize) 
	T_BF_DCF_IF_DATE DateTimeOriginal;		// DateTimeOriginal
	T_BF_DCF_IF_DATE DateTimeDigital;		// digital date and time
	E_BF_DCF_IF_JPEG_FORMAT jpeg_format;	// jpeg format type
	FW_UCHAR maker[FJ_CUSTOM_EXIF_MAKE_S];		// maker
	FW_UCHAR model[FJ_CUSTOM_EXIF_MODEL_S];		// model
	FW_ULONG bitperpixelNum;		// CompressedBitsPerPixel
	FW_ULONG bitperpixelDen;		// CompressedBitsPerPixel
	FW_ULONG ShutterSpeedNum;		// ShutterSpeedValue
	FW_ULONG ShutterSpeedDen;		// ShutterSpeedValue
	FW_LONG ExposureBiasNum;		// ExposureBias Numerator
	FW_LONG ExposureBiasDen;		// ExposureBias Denominator
	FW_ULONG maker_note_offset;		// MakerNote Offset
	FW_USHORT maker_note_length;		// MakerNote Length
#ifdef CO_BF_DCF_IF_ADD_EXIF_INFO
	FW_UCHAR PrintIM[FJ_CUSTOM_EXIF_PRINTIM_S];		// PrintIM
	FW_USHORT CustomRendered;		// Custom Image Processing
	FW_USHORT Contrast;		// Contrast
	FW_USHORT Saturation;		// Saturation
	FW_UCHAR maker1[FJ_CUSTOM_EXIF_MAKER1_S];		// maker1
	FW_UCHAR model1[FJ_CUSTOM_EXIF_MODEL1_S];		// model1
	T_BF_DCF_IF_DATE date_time1;		// date and time1
#endif
	FW_USHORT main_width;		// Main width of SOF
	FW_USHORT main_lines;		// Main Lines of SOF
	FW_UCHAR ycc_format;		// YCbCr format  0x21 : ycbcr422 , 0x22 : ycbcr420
	FW_UCHAR ycc_format_thum;	// YCbCr format  0x21 : ycbcr422 , 0x22 : ycbcr420
	FW_INT32 byte_order;		// Byte Order
	FW_ULONG exiftag_flg;		// EXIF Tag flag(0-32bit) 0:no-exist 1:exist
	FW_ULONG firstIFD_flg;		// 1st IFD flag(0-32bit) 0:no-exist 1:exist
#ifdef CO_BF_DCF_IF_MPF_ENABLE
	FW_UCHAR  baseline_mp_flag;
	FW_ULONG class1_width;
	FW_ULONG class1_lines;
	FW_UCHAR class1_ycc_format;	// YCbCr format  0x21 : ycbcr422 , 0x22 : ycbcr420
	FW_ULONG class2_width;
	FW_ULONG class2_lines;
	FW_UCHAR class2_ycc_format;	// YCbCr format  0x21 : ycbcr422 , 0x22 : ycbcr420
#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
	T_BF_DCF_IF_EXT_MP_IMAGE_DETAIL ext_mp[E_BF_DCF_IF_MPF_EXT_MP_SELECT_MAX];
	FW_ULONG ext_mp_num;
	FW_UCHAR extended_mp_flag;
	FW_UCHAR extended_mp_type;
	FW_ULONG mpfVersion;
#endif    // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
#endif	// CO_BF_DCF_IF_MPF_ENABLE
#ifdef CO_BF_DCF_IF_GET_GPSTAG
	FW_ULONG gpstag_flg;			// GPS Tag flag(0-31bit) 0:no-exist 1:exist
	FW_UCHAR gpsLatitudeRef[2];		// GPS Latitude Ref ('N':北緯, 'S':南緯)
	FW_ULONG gpsLatitudeDdNum;		// GPS Latitude(dd) Numerator
	FW_ULONG gpsLatitudeDdDen;		// GPS Latitude(dd) Denominator
	FW_ULONG gpsLatitudeMmNum;		// GPS Latitude(mm) Numerator
	FW_ULONG gpsLatitudeMmDen;		// GPS Latitude(mm) Denominator
	FW_ULONG gpsLatitudeSsNum;		// GPS Latitude(ss) Numerator
	FW_ULONG gpsLatitudeSsDen;		// GPS Latitude(ss) Denominator
	FW_UCHAR gpsLongitudeRef[2];	// GPS Longitude Ref ('E':東経, 'W':西経)
	FW_ULONG gpsLongitudeDdNum;		// GPS Longitude(dd) Numerator
	FW_ULONG gpsLongitudeDdDen;		// GPS Longitude(dd) Denominator
	FW_ULONG gpsLongitudeMmNum;		// GPS Longitude(mm) Numerator
	FW_ULONG gpsLongitudeMmDen;		// GPS Longitude(mm) Denominator
	FW_ULONG gpsLongitudeSsNum;		// GPS Longitude(ss) Numerator
	FW_ULONG gpsLongitudeSsDen;		// GPS Longitude(ss) Denominator
	FW_UCHAR gpsImgDirectionRef[2];	// GPS Img Direction Ref ('T':真方位, 'M':磁気方位)
	FW_ULONG gpsImgDirectionNum;	// GPS Img Direction Numerator
	FW_ULONG gpsImgDirectionDen;	// GPS Img Direction Denominator
#endif	// CO_BF_DCF_IF_GET_GPSTAG
	E_BF_DCF_IF_APP11_FLG app11_flg;	// APP11 Marker Existence
} T_BF_DCF_IF_IMAGE_DETAIL;

#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
/**
 * @brief save data for jpeg(MPF ScreenNail).
 * @note none
 * @attention none.
 */
typedef struct {
	FW_ULONG  screen_addr;
	FW_ULONG  screen_size;
	FW_ULONG  thumb_addr;
	FW_ULONG  thumb_size;
	FW_USHORT screen_width;
	FW_USHORT screen_lines;
} T_BF_DCF_IF_SCREEN_DATA;
#endif	// CO_BF_DCF_IF_MPF_ENABLE

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE	// for Disparity
/**
 * @brief save data for mpo.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_ULONG main_addr;
	FW_ULONG main_size;
	FW_ULONG thumb_addr;
	FW_ULONG thumb_size;
	FW_ULONG screen_addr;
	FW_ULONG screen_size;
	T_BF_DCF_IF_EXIF_INFO exif_info;
	T_BF_DCF_IF_SCREEN_DATA screen_data1;
	T_BF_DCF_IF_SCREEN_DATA screen_data2;
} T_BF_DCF_IF_EXT_MP_DATA;	
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

/**
 * @brief save data for jpeg.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_ULONG jpg_idx;
	FW_ULONG main_addr;
	FW_ULONG main_size;
	FW_ULONG thumb_addr;
	FW_ULONG thumb_size;
	FW_ULONG screen_addr;
	FW_ULONG screen_size;
	T_BF_DCF_IF_EXIF_INFO exif_info;
#ifdef CO_BF_DCF_IF_MPF_ENABLE	// for MPF
	T_BF_DCF_IF_SCREEN_DATA screen_data1;
	T_BF_DCF_IF_SCREEN_DATA screen_data2;
#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE	// for Disparity
	T_BF_DCF_IF_EXT_MP_DATA ext_mp[E_BF_DCF_IF_MPF_EXT_MP_SELECT_MAX];
	FW_ULONG ext_mp_num;
	FW_CHAR mp_type;
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
#endif	// CO_BF_DCF_IF_MPF_ENABLE
} T_BF_DCF_IF_JPEG_DATA;

typedef T_BF_DCF_IF_JPEG_DATA T_BF_DCF_IF_TIFF_DATA;

/**
 * @brief save data for raw.
 * @note none
 * @attention none.
 */
typedef struct {
	E_BF_DCF_IF_DATA_TYPE	data_type;
	FW_ULONG				main_idx;
	FW_ULONG main_addr;
	FW_ULONG main_size;
} T_BF_DCF_IF_RAW_DATA;


/**
 * @brief save data for thm.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_ULONG thumb_addr;
	FW_ULONG thumb_size;
	T_BF_DCF_IF_EXIF_INFO exif_info;
	FW_CHAR thm_file_path[64];
} T_BF_DCF_IF_THM_DATA;

/**
 * @brief save data for debug.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_ULONG main_addr;
	FW_ULONG main_size;
	FW_CHAR ext[D_BF_DCF_IF_EXT_MAX];
} T_BF_DCF_IF_DEBUG_DATA;


/**
 * @brief clsoe data.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX file_index;
	FW_INT32 file_id;
	E_BF_DCF_IF_REMOVE_FLG remove_flg;
	FW_LONG file_type;		// file type(jpeg,raw,wav,mpeg)
} T_BF_DCF_IF_CLOSE_DATA;

/**
 * @brief image load data.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX file_index;
	FW_ULONG img_addr;
	FW_ULONG img_size;
	FW_LONG file_type;		// file type(jpeg,raw,wav,mpeg)
	FW_ULONG file_offset;
} T_BF_DCF_IF_IMAGE_LOAD_DATA;

/**
 * @brief file open data.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX file_index;
	FW_LONG file_type;		// file type(jpeg,raw,wav,mpeg)
} T_BF_DCF_IF_OPEN_FILE_DATA;

/**
 * @brief resize data.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX file_index;
	FW_LONG file_type;		// file type(jpeg,raw,wav,mpeg)
	FW_ULONG main_addr;
	FW_ULONG main_size;
	FW_ULONG thumb_addr;
	FW_ULONG thumb_size;
	FW_ULONG jpeg_addr;
	FW_ULONG jpeg_size;
	FW_USHORT width;
	FW_USHORT lines;
	E_BF_DCF_IF_CREATE create_flg;
	FW_CHAR copy_flag;
	T_BF_DCF_IF_DATE date;
#ifdef CO_BF_DCF_IF_MPF_ENABLE
	FW_ULONG  class1_size;
	FW_ULONG  class1_addr;
	FW_USHORT class1_width;
	FW_USHORT class1_lines;
	FW_ULONG  class1_thumb_size;
	FW_ULONG  class1_thumb_addr;
	FW_ULONG  class2_size;
	FW_ULONG  class2_addr;
	FW_USHORT class2_width;
	FW_USHORT class2_lines;
	FW_ULONG  class2_thumb_size;
	FW_ULONG  class2_thumb_addr;
#endif	// CO_BF_DCF_IF_MPF_ENABLE
} T_BF_DCF_IF_RESIZE_PARAM;

/**
 * @brief rotate parameter.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX file_index;
	FW_LONG file_type;		// file type(jpeg,raw,wav,mpeg)
	E_BF_DCF_IF_ROTATE rotate;
	E_BF_DCF_IF_ROTATE rotate_thumb;
	E_BF_DCF_IF_CREATE create_flg;
	T_BF_DCF_IF_DATE date;
} T_BF_DCF_IF_ROTATE_PARAM;

/**
 * @brief delete data structure.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX file_index;
} T_BF_DCF_IF_DEL_DATA;

/**
 * @brief select delete data structure.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_UINT32 sel_num;
	T_BF_DCF_IF_DEL_DATA sel_list[200];
} T_BF_DCF_IF_DEL_SELECT_LIST;

/**
 * @brief select delete data structure.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX* p_select_list;
	FW_UCHAR select_num;
} T_BF_DCF_IF_DEL_SELECT;

/**
 * @brief attribute change data structure.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX file_index;
	FW_UINT32 attr : 1;
	FW_UINT32 reserve : 15;
} T_BF_DCF_IF_ATTR_DATA;

/**
 * @brief select attribute change data structure.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_UINT32 sel_num;
	T_BF_DCF_IF_ATTR_DATA sel_list[200];
} T_BF_DCF_IF_ATTR_SELECT_LIST;

/**
 * @brief attribute change data structure for all.
 * @note none
 * @attention none.
 */
typedef struct {
	E_BF_MEDIA_STATE_MEDIA_TYPE media_type;
	E_BF_DCF_IF_ATTR all_attr;
} T_BF_DCF_IF_ATTR_ALL_DATA;

/**
 * @brief attribute change data structure for file index type.
 * @note none
 * @attention none.
 */
typedef struct {
	E_BF_DCF_IF_FILE_INDEX_TYPE file_idx_type;
	E_BF_DCF_IF_ATTR all_attr;
} T_BF_DCF_IF_ATTR_IDX_TYPE_DATA;

/**
 * @brief DPOF image data structure.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX file_index;
	FW_USHORT file_type;
} T_BF_DCF_IF_DPOF_IMG_DATA;

/**
 * @brief DPOF param data structure.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_INT32 job_id;
	FW_INT32 param_id;
	FW_INT32 value;
	FW_UCHAR str[32];
} T_BF_DCF_IF_DPOF_PARAM_DATA;

/**
 * @brief DPOF standard data structure.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX file_index;
	FW_UINT32 sheets : 10;
	FW_UINT32 reserve : 2;
} T_BF_DCF_IF_DPOF_STD_DATA;

/**
 * @brief select DPOF standard data structure.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_UINT32 sel_num;
	T_BF_DCF_IF_DPOF_STD_DATA sel_list[200];
} T_BF_DCF_IF_DPOF_STD_SELECT_LIST;

/**
 * @brief DPOF stadnard data structure for all.
 * @note none
 * @attention none.
 */
typedef struct {
	E_BF_MEDIA_STATE_MEDIA_TYPE media_type;
	FW_USHORT sheets;	// sheets
} T_BF_DCF_IF_DPOF_STD_ALL_DATA;

/**
 * @brief DPOF size data structure.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX file_index;
	FW_UINT32 size : 7;
	FW_UINT32 reserve : 9;
} T_BF_DCF_IF_DPOF_SIZ_DATA;

/**
 * @brief select DPOF size data structure.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_UINT32 sel_num;
	T_BF_DCF_IF_DPOF_SIZ_DATA sel_list[200];
} T_BF_DCF_IF_DPOF_SIZ_SELECT_LIST;

/**
 * @brief DPOF size data structure for all.
 * @note none
 * @attention none.
 */
typedef struct {
	E_BF_MEDIA_STATE_MEDIA_TYPE media_type;
	E_BF_DCF_IF_DPOF_SIZE size;	// size
} T_BF_DCF_IF_DPOF_SIZ_ALL_DATA;

/**
 * @brief DPOF file data structure.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_ULONG buf;
	FW_ULONG* size;
}T_BF_DCF_IF_DPOF_FILE;


typedef struct {
	FW_INT32 hdr_param_id;
	FW_CHAR  value[20];
} T_BF_DCF_IF_DPOF_HDR_PARAM;

/**
 * @brief set attribute data.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX file_index;
	FW_CHAR ext[D_BF_DCF_IF_EXT_MAX];
	FW_LONG file_type;		// file type(jpeg,raw,wav,mpeg)
} T_BF_DCF_IF_INDEX_FILE_DATA;


/**
 * @brief set file name parameter.
 * @note none
 * @attention none.
 */
typedef struct {
	E_BF_DCF_IF_FILE_NAME file_name;
	FW_USHORT dirno;
	FW_USHORT fileno;
} T_BF_DCF_IF_FILE_NAME;

/**
 * @brief set free file name parameter.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_INT32 check_dir;
	FW_CHAR free_char[8];
	FW_INT32 operation;
} T_BF_DCF_IF_FREE_FILE_NAME;

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
/**
 * @brief load individual image info.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_ULONG main_size;
	FW_ULONG main_offset;
	FW_ULONG thumb_size;
	FW_ULONG thumb_offset;
	FW_ULONG class1_size;
	FW_ULONG class1_offset;
	FW_ULONG class1_thumb_size;
	FW_ULONG class1_thumb_offset;
	FW_ULONG class2_size;
	FW_ULONG class2_offset;
	FW_ULONG class2_thumb_size;
	FW_ULONG class2_thumb_offset;
} T_BF_DCF_IF_LOAD_EXT_MP_IMAGE_DATA;	
#endif    // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

/**
 * @brief load image info.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_ULONG thumb_offset;
	FW_ULONG thumb_size;
	FW_ULONG main_offset; // for tiff
#ifdef CO_BF_DCF_IF_MPF_ENABLE
	FW_ULONG main_size;
	FW_ULONG class1_size;
	FW_ULONG class1_offset;
	FW_ULONG class1_thumb_size;
	FW_ULONG class1_thumb_offset;
	FW_ULONG class2_size;
	FW_ULONG class2_offset;
	FW_ULONG class2_thumb_size;
	FW_ULONG class2_thumb_offset;
#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
	T_BF_DCF_IF_LOAD_EXT_MP_IMAGE_DATA ext_mp[E_BF_DCF_IF_MPF_EXT_MP_SELECT_MAX];
	FW_ULONG ext_mp_num;
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
#endif	// CO_BF_DCF_IF_MPF_ENABLE

} T_BF_DCF_IF_LOAD_IMAGE_DATA;

/**
 * @brief index list data.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_USHORT search_index;
	E_BF_DCF_IF_FILE_INDEX_TYPE file_idx_type;
	FW_UCHAR index_list_num;
	FW_USHORT* index_list_addr;
} T_BF_DCF_IF_INDEX_LIST;

/**
 * @brief copy data.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX tgt_index;
	E_BF_MEDIA_STATE_MEDIA_TYPE dst_media;
	FW_ULONG load_addr;
	FW_ULONG load_area_size;
} T_BF_DCF_IF_COPY_DATA;

/**
 * @brief Quantization Talbe.
 * @note none
 * @attention none.
 */
typedef struct {
	FW_UCHAR qt_flg_y;
	FW_UCHAR qt_y[64];
	FW_UCHAR qt_flg_cb;
	FW_UCHAR qt_cb[64];
	FW_UCHAR qt_flg_cr;
	FW_UCHAR qt_cr[64];
} T_BF_DCF_IF_JPEG_QT;


/**
 * @brief get qt data.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX tgt_index;
	T_BF_DCF_IF_JPEG_QT* qt;
	E_BF_DCF_IF_QT_TYPE qt_type;
} T_BF_DCF_IF_GET_QT_DATA;

/**
 * @brief mark bit change data structure.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX file_index;
	FW_LONG file_type;		// file type(jpeg,raw,wav,mpeg)
	FW_CHAR* fpath; 
	FW_UCHAR* attr;
	FW_USHORT* date;
	FW_USHORT* time;
	FW_ULONG* file_size;
} T_BF_DCF_IF_FSTAT_DATA;

/**
 * @brief file index and file name.
 * @note none
 * @attention none.
 */
typedef struct {
	T_BF_DCF_IF_FILE_INDEX* file_index;
	FW_CHAR* file_path;
} T_BF_DCF_IF_INDEX_FILE_NAME;

/*=============================================================================
; List Structure
=============================================================================*/
/**
 * @breif List structure
 * @note none
 * @attention none.
 */
typedef struct t_LIST {
	struct t_LIST* pNext;
} T_BF_DCF_IF_LIST;


/*=============================================================================
; Queue Structure
=============================================================================*/
/**
 * @breif Queue structure
 * @note none
 * @attention none.
 */
typedef struct t_QUEUE {
	struct t_QUEUE* pNext;
	struct t_QUEUE* pPrevious;
} T_BF_DCF_IF_QUEUE;

/**
 * @breif structure for BF_Dcf_If_Set_Dir_Db function
 * @note none
 * @attention none.
 */
typedef struct {
	FW_UCHAR act_mode;
	FW_LONG index;
} T_BF_DCF_IF_SET_CUR_DIR;

/**
 * @breif structure for BF_Dcf_If_Write_Obj_Attr function
 * @note none
 * @attention none.
 */
typedef struct {
	FW_LONG index;
	FW_UCHAR attr;
} T_BF_DCF_IF_WRT_OBJ_ATTR;


/**
 * @brief structure for BF_Dcf_If_Create_Dir function
 * @note none
 * @attention none.
 */
typedef struct {
	FW_USHORT dirNum;
	FW_UCHAR dirNameIndex;
} T_BF_DCF_IF_CREATE_CUR_DIR;


/**
 * @brief structure for DCF_DB
 * @note none
 * @attention none.
 */
#if 1
typedef struct {
	FW_INT32 ret_ercd;
	FW_SHORT* p_dirCnt;
	FW_CHAR* topDirName;
	FW_USHORT* p_date;
	FW_USHORT* p_time;
	FW_UCHAR* p_sec10m;
	FW_CHAR* dirName;
	FW_SHORT* p_dirIndex;
	FW_SHORT* p_objectCnt;
	FW_UCHAR* p_extType;
	FW_CHAR* objName;
	FW_UCHAR* p_attr;
} T_BF_DCF_IF_DB_INFO;
#else
typedef struct {
	FW_INT32 ret_ercd;
	struct {
		FW_SHORT dirCnt;
		FW_CHAR topDirName[D_BF_DCF_IF_TOP_DIR_NAME_MAX];
		union {
			FW_UCHAR bits;
			struct {
				FW_ULONG dirCnt     : 1;
				FW_ULONG topDirName : 1;
			} bit;
		} effectBit;
	} dirCb;
	struct {
		FW_USHORT date;
		FW_USHORT time1;
		FW_CHAR dirName[D_BF_DCF_IF_DIR_NAME_MAX];
		union {
			FW_UCHAR bits;
			struct {
				FW_ULONG date    : 1;
				FW_ULONG time1   : 1;
				FW_ULONG dirName : 1;
			} bit;
		} effectBit;
	} dirInfo;
	struct {
		FW_SHORT dirIndex;
		FW_SHORT ObjectCnt;
		union {
			FW_UCHAR bits;
			struct {
				FW_ULONG dirIndex  : 1;
				FW_ULONG ObjectCnt : 1;
			} bit;
		} effectBit;
	} dBBuf;
	struct {
		FW_SHORT objIndex;
		FW_UCHAR extType;
		FW_CHAR objName[D_BF_DCF_IF_OBJ_ONLY_NAME_MAX];
		FW_UCHAR attr;
		union {
			FW_UCHAR bits;
			struct {
				FW_ULONG objIndex    : 1;
				FW_ULONG extType     : 1;
				FW_ULONG objName     : 1;
				FW_ULONG attr        : 1;
			} bit;
		} effectBit;
	} ObjInfo;
} T_BF_DCF_IF_DB_INFO;
#endif

typedef struct {
	T_BF_DCF_IF_FILE_INDEX file_index;
	T_BF_FS_IF_STAT*     fsif_stat;
} T_BF_DCF_IF_FSTAT;

/**
 * @breif structure for BF_Dcf_If_Get_Dir_File_Exist function
 * @note none
 * @attention none.
 */
typedef struct {
	FW_USHORT dirindex;
	FW_UCHAR* file_exist_flag;
} T_BF_DCF_IF_DIR_FILE_EXIST;

/**
 * @breif structure for BF_Dcf_If_Make_New_Thm_File_Path function
 * @note none
 * @attention none.
 */
typedef struct {
	FW_CHAR ext[D_BF_DCF_IF_EXT_MAX];
	FW_CHAR main_filepath[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
} T_BF_DCF_IF_MAKE_THM_FILE_PATH_PARAM;

/**
 * @brief set year and month.
 */
typedef struct {
	FW_USHORT year;
	FW_UCHAR month;
} T_BF_DCF_IF_YEAR_MONTH;

/**
 * @breif structure for BF_Dcf_If_Get_Date_Index_By_File_Num function
 * @note none
 * @attention none.
 */
typedef struct {
	FW_CHAR dcf_root_dir_name[D_BF_DCF_IF_DIR_NAME_MAX];
	FW_USHORT DirNum;
	FW_USHORT FileNum;
} T_BF_DCF_IF_GET_DATE_IDX_BY_FILE_NUM;

/**
 * @breif structure for BF_Dcf_If_Updata_File function
 * @note none
 * @attention none.
 */
typedef struct {
	E_BF_DCF_IF_UPDATE_KIND kind;
	FW_CHAR file_path[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
} T_BF_DCF_IF_UPDATA_FILE;

/**
 * @breif structure for BF_Dcf_If_Write_Date_Db_Obj_Filter_Attr function
 * @note none
 * @attention none.
 */
typedef struct {
	FW_CHAR file_path[D_BF_DCF_IF_FULLPASS_FILENAME_MAX];
	FJ_DCF_DATE_FILTER_TYPE filter_attr;
} T_BF_DCF_IF_WRITE_FILTER_ATTR;

/**
 * @breif structure for BF_Dcf_If_Update_Date_Db_Obj_Filter_Attr function
 * @note none
 * @attention none.
 */
typedef struct {
	FW_USHORT DateIndex;
	FJ_DCF_DATE_FILTER_TYPE filter_attr;
} T_BF_DCF_IF_UPDATE_FILTER_ATTR;

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
#ifdef ASSERT_USED
#define dcf_if_assert(x)	BF_Dcf_If_Assert(x, __FILE__, __LINE__)
#else
#define dcf_if_assert(x)
#endif

/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
#if 1	// for DCF-task
FW_INT32 BF_Dcf_If_End_Task_Mdf(FW_VOID);
FW_INT32 BF_Dcf_If_Set_Cur_Drive(FW_VOID);
#endif

#ifdef CO_BF_DCF_IF_MPF_DISPARITY_ENABLE
FW_INT32 BF_Dcf_If_Save_Ext(DCF_ENTRY entry_func, T_BF_DCF_IF_JPEG_DATA* dcf_ext_data);
FW_INT32 BF_Dcf_If_Select_Ext_No(FW_UCHAR mp_no);
FW_INT32 BF_Dcf_If_Get_Exif_Info_Ext(FW_LONG file_index, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data);
FW_INT32 BF_Dcf_If_Load_Ext_Monitor_Thumb(FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG offset, FW_ULONG* length);
FW_INT32 BF_Dcf_If_Load_Ext_Thumb(FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG* length, T_BF_DCF_IF_IMAGE_DETAIL* img_detail);
FW_INT32 BF_Dcf_If_Start_Div_Load_Ext(DCF_ENTRY entry_func, FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG offset);
FW_INT32 BF_Dcf_If_Start_Div_Load_Ext_Monitor(DCF_ENTRY entry_func, FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG offset);
#endif     // CO_BF_DCF_IF_MPF_DISPARITY_ENABLE

FW_INT32 BF_Dcf_If_Format_As(DCF_ENTRY entry_func, FW_INT32 media_id);
FW_INT32 BF_Dcf_If_Create_DataBase(FW_VOID);
FW_INT32 BF_Dcf_If_Start_Used_Media_As(DCF_ENTRY entry_func, FW_INT32 media_type);
FW_INT32 BF_Dcf_If_End_Used_Media_As(DCF_ENTRY entry_func, FW_INT32 media_type);
FW_INT32 BF_Dcf_If_Save_Jpeg_As(DCF_ENTRY entry_func, T_BF_DCF_IF_JPEG_DATA* dcf_jpg_data);
FW_INT32 BF_Dcf_If_Save_Raw_As(DCF_ENTRY entry_func, T_BF_DCF_IF_RAW_DATA* dcf_raw_data);
FW_INT32 BF_Dcf_If_Save_Thm_As(FS_ENTRY entry_func, T_BF_DCF_IF_JPEG_DATA* dcf_jpg_data, FW_CHAR* avi_file_path);

FW_INT32 BF_Dcf_If_Get_Exif_Info_Jpeg(FW_LONG file_index, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data);
FW_INT32 BF_Dcf_If_Get_Exif_Info_Jpeg_Name(FW_CHAR* file_path, T_BF_DCF_IF_IMAGE_DETAIL* img_detail, T_BF_DCF_IF_LOAD_IMAGE_DATA* load_image_data);

FW_INT32 BF_Dcf_If_Load_Vga(FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG vga_offset, FW_ULONG* length);
FW_INT32 BF_Dcf_If_Load_Monitor_Thumb(FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG offset, FW_ULONG* length);
FW_INT32 BF_Dcf_If_Load_Thumb_Jpeg(FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG* length, T_BF_DCF_IF_IMAGE_DETAIL* img_detail);

FW_INT32 BF_Dcf_If_Start_Div_Load_Jpeg(DCF_ENTRY entry_func, FW_LONG file_index, FW_ULONG addr, FW_ULONG size);
FW_INT32 BF_Dcf_If_Start_Div_Load_Monitor(DCF_ENTRY entry_func, FW_LONG file_index, FW_ULONG addr, FW_ULONG size, FW_ULONG offset);
FW_INT32 BF_Dcf_If_Get_Div_Load_Image_Result(FW_ULONG* length);
FW_INT32 BF_Dcf_If_Stop_Div_Load_Image(FW_VOID);
FW_INT32 BF_Dcf_If_Save_Resize(T_BF_DCF_IF_RESIZE_PARAM resize_param);

FW_INT32 BF_Dcf_If_Update_Rotate(FW_LONG file_index, FW_LONG file_type, E_BF_DCF_IF_ROTATE rotate, E_BF_DCF_IF_ROTATE rotate_thumb, E_BF_DCF_IF_CREATE c_flg, T_BF_DCF_IF_DATE date);
FW_INT32 BF_Dcf_If_Del_Single_Obj(FW_LONG file_index);
FW_INT32 BF_Dcf_If_Del_All_Obj_In_Current_Dir(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);
FW_INT32 BF_Dcf_If_Del_All_Obj(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);

FW_INT32 BF_Dcf_If_Stop_Async_Process(FW_VOID);

FW_INT32 BF_Dcf_If_Set_Single_Obj_Attr(FW_LONG file_index, E_BF_DCF_IF_ATTR attr);

FW_INT32 BF_Dcf_If_Set_All_Obj_Attr_In_Current_Dir(E_BF_MEDIA_STATE_MEDIA_TYPE media_type, E_BF_DCF_IF_ATTR attr);
FW_INT32 BF_Dcf_If_Set_All_Obj_Attr(E_BF_MEDIA_STATE_MEDIA_TYPE media_type, E_BF_DCF_IF_ATTR attr);

FW_INT32 BF_Dcf_If_Get_Fstat(FW_LONG file_index, FW_LONG file_type, FW_CHAR* fpath, FW_UCHAR* attr, FW_USHORT* date, FW_USHORT* time, FW_ULONG* file_size);

FW_INT32 DCFIF_Get_Jpeg_Entry(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);
FW_INT32 BF_Dcf_If_Get_File_Id(FW_CHAR* file_path, FW_INT32* file_id);

FW_INT32 BF_Dcf_If_Get_Entry(E_BF_MEDIA_STATE_MEDIA_TYPE media_type);
FW_INT32 BF_Dcf_If_Get_File_Info(FW_LONG file_index, T_BF_DCF_IF_FILE_INFO* file_info);
FW_INT32 BF_Dcf_If_Get_File_Type(FW_LONG file_index, FW_SHORT* file_type);
FW_INT32 BF_Dcf_If_Make_New_File_Path(const FW_CHAR* ext, FW_CHAR* make_filepath);
FW_INT32 BF_Dcf_If_Make_New_Thm_File_Path(const FW_CHAR* ext, const FW_CHAR* main_filepath, FW_CHAR* make_filepath);

FW_INT32 BF_Dcf_If_Make_Latest_File_Path(FW_LONG file_type, FW_CHAR* make_filepath);

FW_INT32 BF_Dcf_If_Make_Index_File_Path(FW_LONG file_index, FW_LONG file_type, FW_CHAR* make_filepath);
FW_INT32 BF_Dcf_If_Get_Index_File_Path(FW_LONG file_index, const FW_CHAR* ext, FW_CHAR* filepath);

FW_INT32 BF_Dcf_If_Updata_File(E_BF_DCF_IF_UPDATE_KIND kind, FW_CHAR* filepath);
FW_INT32 BF_Dcf_If_Updata_Option_File(FW_CHAR* filepath);
FW_INT32 BF_Dcf_If_Set_Series_File_No(DCF_ENTRY entry_func, FW_USHORT dirno, FW_USHORT fileno);
E_BF_MEDIA_STATE_MEDIA_TYPE BF_Dcf_If_Get_Series_File_No(FW_USHORT* dirno, FW_USHORT* fileno, E_BF_DCF_IF_LATEST_TYPE type);

FW_INT32 BF_Dcf_If_Get_Jpeg_Quant_Table(FW_LONG tgt_index, T_BF_DCF_IF_JPEG_QT* qt, E_BF_DCF_IF_QT_TYPE qt_type);

//FW_INT32 BF_Dcf_If_Get_Duplicate_File_No(FW_USHORT *dirno, FW_USHORT *fileno );
FW_INT32 BF_Dcf_If_Set_Free_File_Name_As(DCF_ENTRY entry_func, FW_CHAR check_dir, FW_CHAR* free_char, FW_CHAR operation);


FW_VOID BF_Dcf_If_Entry_Dummy(FW_INT32 ret);
FW_INT32 BF_Dcf_If_Reset_Db(DCF_ENTRY entry_func);
FW_INT32 BF_Dcf_If_Set_Dir_Db(DCF_ENTRY entry_func, const FW_CHAR* dir_fullpass_name);
FW_INT32 BF_Dcf_If_Read_Dir_Total_Count(FW_SHORT* dir_cnt);
FW_INT32 BF_Dcf_If_Get_Index_By_Dir_Name(const FW_CHAR* dir_name, FW_LONG* index);
FW_INT32 BF_Dcf_If_Read_Dir_Name(FW_LONG index, FW_CHAR* dir_name);
FW_INT32 BF_Dcf_If_Read_Dir_Duplicate_Status(FW_LONG index, FW_CHAR* dir_dupstatus);
FW_INT32 BF_Dcf_If_Read_Dir_Date_Time(FW_LONG index, FW_USHORT* dir_created_date, FW_USHORT* dir_created_time);
FW_INT32 BF_Dcf_If_Get_Max_File_Num_As_Dir_Num(FW_SHORT* file_no);
FW_INT32 BF_Dcf_If_Set_Current_Dir(DCF_ENTRY entry_func, FW_UCHAR act_mode, FW_LONG index);
FW_INT32 BF_Dcf_If_Get_Dir_Index(FW_LONG index, FW_LONG* index_num);
FW_INT32 BF_Dcf_If_Read_Obj_Count(FW_LONG index, FW_SHORT* obj_count);
FW_INT32 BF_Dcf_If_Read_Obj_Name(FW_LONG index, FW_CHAR* obj_name);
FW_INT32 BF_Dcf_If_Read_Dir_Obj_Name(FW_LONG index, FW_CHAR* dir_obj_name);
FW_INT32 BF_Dcf_If_Read_Obj_Info(FW_LONG index, T_BF_DCF_IF_DCFDB_OBJ_INFO* obj_info);
FW_INT32 BF_Dcf_If_Write_Obj_Attr(FW_LONG index, FW_UCHAR obj_attr);
FW_INT32 BF_Dcf_If_Get_Index_By_Obj_Name(const FW_CHAR* obj_name, FW_LONG* index);
FW_INT32 BF_Dcf_If_Set_Allowed_Ext_List(const FW_CHAR* allowed_ext_list);
FW_INT32 BF_Dcf_If_Read_Allowed_Ext_Count(FW_LONG file_index, FW_UCHAR* allowed_ext_count);
FW_INT32 BF_Dcf_If_Get_Type_By_Ext_Name(const FW_CHAR* ext_name, FW_UCHAR* ext_type);
FW_INT32 BF_Dcf_If_Get_Ext_Name_By_Type(FW_UCHAR ext_type, FW_CHAR* allowed_ext_name);
FW_INT32 BF_Dcf_If_Get_Full_Path_By_File_Index( FW_ULONG file_index, FW_CHAR* full_path);

BOOL BF_Dcf_If_Is_File_Type(const FW_CHAR* ext_name, FW_SHORT file_type);
E_BF_DCF_IF_FILE_TYPE BF_Dcf_If_Convert_File_Type(FW_LONG file_type);
FW_INT32 BF_Dcf_If_Get_Obj_Info(FW_LONG file_index, T_BF_DCF_IF_DB_INFO* dcfdb_info);
FW_INT32 BF_Dcf_If_Create_Dir(FW_USHORT dirNum, FW_UCHAR dirNameIndex);
FW_INT32 BF_Dcf_If_Get_First_Free_File_No(FW_USHORT* fileNum);
FW_INT32 BF_Dcf_If_Get_New_Copy_Dir_No(FW_USHORT* dirNum);
FW_INT32 BF_Dcf_If_Get_Last_File_Number(FW_LONG index, FW_USHORT* file_num);
FW_INT32 BF_Dcf_If_Get_Dir_File_Exist(USHORT dir_index, UCHAR* file_exist);
FW_INT32 BF_Dcf_If_Del_Single_Dcf_Dir(USHORT dir_index);
FW_INT32 BF_Dcf_If_Del_All_Dcf_Dir(FW_VOID);

#ifdef ASSERT_USED
FW_VOID BF_Dcf_If_Assert(INT32 test, CHAR* filename, INT32 line);
#endif

/* utility */
FW_VOID BF_Dcf_If_Unic_Queue(T_BF_DCF_IF_QUEUE* queue);
FW_VOID BF_Dcf_If_Add_Queue_Next(T_BF_DCF_IF_QUEUE* controlqueue, T_BF_DCF_IF_QUEUE* aqueue);
FW_VOID BF_Dcf_If_Add_Queue_Previous(T_BF_DCF_IF_QUEUE* controlqueue, T_BF_DCF_IF_QUEUE* aqueue);
FW_VOID BF_Dcf_If_Delete_Queue(T_BF_DCF_IF_QUEUE* queue);
FW_VOID BF_Dcf_If_Make_Free_List(T_BF_DCF_IF_LIST** pFreeList, T_BF_DCF_IF_LIST* pStartArea, FW_USHORT num, FW_USHORT size);

FW_INT32 BF_Dcf_If_Fstat(FW_LONG file_index, T_BF_FS_IF_STAT* fsif_stat);

FW_INT32 BF_Dcf_If_Get_Dcf_Key(FW_ULONG operation, FW_USHORT* dirno, FW_USHORT* fileno);
FW_INT32 BF_Dcf_If_Get_Make_Dir_Name(FW_CHAR* dirname);
FW_INT32 BF_Dcf_If_Get_Make_File_Name(FW_CHAR* filename);

#if 1	/* for Multi-Delete */
FW_INT32 BF_Dcf_If_Init_Select_List(FW_VOID);
FW_INT32 BF_Dcf_If_Add_File_To_Select_List(FW_LONG file_index);
FW_INT32 BF_Dcf_If_Remove_File_From_Select_List(FW_LONG file_index);
FW_INT32 BF_Dcf_If_Execute_Multi_Delete(FW_VOID);
#endif	/* for Multi-Delete */
#endif
