/**
 * @file		fj_exif.h
 * @brief		EXIF Controller.
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#ifndef _FJ_EXIF_H
#define _FJ_EXIF_H

//#include "fj_cstdlib.h"
#include "fj_custom.h"
#include "fj_memory.h"

///////////////////////////////////////////////////////////////////////////////
// overview
///////////////////////////////////////////////////////////////////////////////
/** @addtogroup fj_exif_overview
@{
	The EXIF controls can be achieved by using this API.<br>
@}*//* --- end of fj_sample_overview */

///////////////////////////////////////////////////////////////////////////////
// sequence
///////////////////////////////////////////////////////////////////////////////
/** @addtogroup fj_exif_sequence
@{
	- @ref Get_EXIF_tag_parameter
	- @ref Create_EXIF_information
	- @ref Parse_EXIF_MakerNote

	<hr>
	@section Get_EXIF_tag_parameter Get EXIF tag parameter sequence
	@image html fj_exif_Get_EXIF_tag_param.png

	@section Create_EXIF_information Create EXIF information sequence
	@image html fj_exif_Create_EXIF_information.png
	Refer to Capture sequence of BaseFWPF.chm for details of Normal Capture.

	@section Parse_EXIF_MakerNote Parse EXIF MakerNote sequence
	@image html fj_exif_parse_EXIF_MakerNote.png
	Refer to PlayBack sequence of BaseFWPF.chm for details of SingleDisplay.

@}*//* --- end of fj_exif_sequence */

///////////////////////////////////////////////////////////////////////////////
// definition
///////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
/** @weakgroup fj_exif_definition
@{*/
// Exif Tag
#define FJ_IFD0_TAG_IMGDESC_			0x010E	/**< Optional */
#define FJ_IFD0_TAG_MAKE_				0x010F	/**< Optional */
#define FJ_IFD0_TAG_MODEL_				0x0110	/**< Optional */
#define FJ_IFD0_TAG_ORIENT_				0x0112	/**< Optional */
#define FJ_IFD0_TAG_XRES_				0x011A	/**< Mandatory */
#define FJ_IFD0_TAG_YRES_				0x011B	/**< Mandatory */
#define FJ_IFD0_TAG_RESUNIT_			0x0128	/**< Mandatory */
#define FJ_IFD0_TAG_SOFT_				0x0131	/**< Optional */
#define FJ_IFD0_TAG_DATETIME_			0x0132	/**< Optional */
#define FJ_IFD0_TAG_YCBCRPOS_			0x0213	/**< Mandatory */
#define FJ_IFD0_TAG_COPYRIGHT_			0x8298	/**< Optional */
#define FJ_IFD0_TAG_EXIFPTR_			0x8769	/**< Mandatory */
//#ifdef CO_BF_DCF_IF_SET_GPSTAG
#define FJ_IFD0_TAG_GPSPTR_				0x8825	/**< Mandatory */
//#endif
#define FJ_IFD0_TAG_PRINTIM_			0xC4A5	/**< Optional */

#define FJ_EXIF_TAG_EXPTIME_			0x829A	/**< Optional */
#define FJ_EXIF_TAG_FNUM_				0x829D	/**< Optional */
#define FJ_EXIF_TAG_EXPPROG_			0x8822	/**< Optional */
#define FJ_EXIF_TAG_ISOSPEEDRATE_		0x8827	/**< Optional */
#define FJ_EXIF_TAG_EXIFVER_			0x9000	/**< Mandatory */
#define FJ_EXIF_TAG_DATETIMEORIG_		0x9003	/**< Optional */
#define FJ_EXIF_TAG_DATETIMEDITI_		0x9004	/**< Optional */
#define FJ_EXIF_TAG_COMPCONFIGURE_		0x9101	/**< Mandatory */
#define FJ_EXIF_TAG_COMPRESSBPP_		0x9102	/**< Optional */
#define FJ_EXIF_TAG_SHUTTER_			0x9201	/**< Optional */
#define FJ_EXIF_TAG_APERTURE_			0x9202	/**< Optional */
#define FJ_EXIF_TAG_EXPBIAS_			0x9204	/**< Optional */
#define FJ_EXIF_TAG_MAXAPTURE_			0x9205	/**< Optional */
#define FJ_EXIF_TAG_METERMODE_			0x9207	/**< Optional */
#define FJ_EXIF_TAG_LIGHTSOURCE_		0x9208	/**< Optional */
#define FJ_EXIF_TAG_FLASH_				0x9209	/**< Optional */
#define FJ_EXIF_TAG_FOCALLEN_			0x920A	/**< Optional */
#define FJ_EXIF_TAG_MAKERNOTE_			0x927C	/**< Optional */
#define FJ_EXIF_TAG_FLRESHPIXVER_		0xA000	/**< Mandatory */
#define FJ_EXIF_TAG_COLORSPACE_			0xA001	/**< Mandatory */
#define FJ_EXIF_TAG_PEXELXDIM_			0xA002	/**< Mandatory */
#define FJ_EXIF_TAG_PEXELYDIM_			0xA003	/**< Mandatory */
#define FJ_EXIF_TAG_ITOPIFDPTR_			0xA005	/**< Optional */
#define FJ_EXIF_TAG_SENSOR_				0xA217	/**< Optional */
#define FJ_EXIF_TAG_FILESOURCE_			0xA300	/**< Optional */
#define FJ_EXIF_TAG_SENCETYPE_			0xA301	/**< Optional */
#define FJ_EXIF_TAG_CUSTOMRENDERED_		0xA401	/**< Optional */
#define FJ_EXIF_TAG_EXPOSUREMODE_		0xA402	/**< Optional */
#define FJ_EXIF_TAG_WB_					0xA403	/**< Optional */
#define FJ_EXIF_TAG_DZOOM_				0xA404	/**< Optional */
#define FJ_EXIF_TAG_FOCALLENGTH35mm_	0xA405	/**< Optional */
#define FJ_EXIF_TAG_CAPTURETYPE_		0xA406	/**< Optional */
#define FJ_EXIF_TAG_CONTRAST_			0xA408	/**< Optional */
#define FJ_EXIF_TAG_SATURATION_			0xA409	/**< Optional */
#define FJ_EXIF_TAG_SHARPNESS_			0xA40A	/**< Optional */
#define FJ_EXIF_TAG_IMAGEUID_			0xA420	/**< Optional for MPF(Class1/Class2) */

#define FJ_IFD1_TAG_COMPRESS_			0x0103	/**< Mandatory */
#define FJ_IFD1_TAG_MAKER1_				0x010F	/**< Optional */
#define FJ_IFD1_TAG_MODEL1_				0x0110	/**< Optional */
#define FJ_IFD1_TAG_ORIENT_				0x0112	/**< Optional */
#define FJ_IFD1_TAG_XRES_				0x011A	/**< Mandatory */
#define FJ_IFD1_TAG_YRES_				0x011B	/**< Mandatory */
#define FJ_IFD1_TAG_RESUINT_			0x0128	/**< Mandatory */
#define FJ_IFD1_TAG_DATETIME1_			0x0132	/**< Optional */
#define FJ_IFD1_TAG_JPG_INTERCHGFMT_	0x0201	/**< Mandatory */
#define FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_	0x0202	/**< Mandatory */

#define FJ_GPS_TAG_GPSVERSIONID_		0x0000	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSLATITUDEREF_		0x0001	/**< Optional */
#define FJ_GPS_TAG_GPSLATITUDE_			0x0002	/**< Optional */
#define FJ_GPS_TAG_GPSLONGITUDEREF_		0x0003	/**< Optional */
#define FJ_GPS_TAG_GPSLONGITUDE_		0x0004	/**< Optional */
#define FJ_GPS_TAG_GPSALTITUDEREF_		0x0005	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSALTITUDE_			0x0006	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSTIMESTAMP_		0x0007	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSSATELLITES_		0x0008	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSSTATUS_			0x0009	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSMEASUREMODE_		0x000A	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSDOP_				0x000B	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSSPEEDREF_			0x000C	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSSPEED_			0x000D	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSTRACKREF_			0x000E	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSTRACK_			0x000F	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSIMGDIRECTIONREF_	0x0010	/**< Optional */
#define FJ_GPS_TAG_GPSIMGDIRECTION_		0x0011	/**< Optional */
#define FJ_GPS_TAG_GPSMAPDATUM_			0x0012	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSDESTLATITUDEREF_	0x0013	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSDESTLATITUDE_		0x0014	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSDESTLONGITUDEREF_	0x0015	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSDESTLONGITUDE_	0x0016	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSDESTBEARINGREF_	0x0017	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSDESTBEARING_		0x0018	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSDESTDISTANCEREF_	0x0019	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSDESTDISTANCE_		0x001A	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSPROCESSINGMETHOD_	0x001B	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSAREAINFORMATION_	0x001C	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSDATESTAMP_		0x001D	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSDIFFERENTIAL_		0x001E	/**< Optional (not supported) */
#define FJ_GPS_TAG_GPSHPOSITIONERR_		0x001F	/**< Optional (not supported) */

#define FJ_ITOP_TAG_ITOPINDEX_			0x0001	/**< Optional */
#define FJ_ITOP_TAG_ITOPVERSION_		0x0002	/**< Optional */

#define FJ_EXIF_TAG_MAKE_FLAG				(0x00000001)	/**< Tag ID of EXIF 0x010F */
#define FJ_EXIF_TAG_MODEL_FLAG				(0x00000002)	/**< Tag ID of EXIF 0x0110 */
#define FJ_EXIF_TAG_ORIENT_FLAG				(0x00000004)	/**< Tag ID of EXIF 0x0112 */
#define FJ_EXIF_TAG_DATETIME_FLAG			(0x00000008)	/**< Tag ID of EXIF 0x0132 */
#define FJ_EXIF_TAG_EXIFPTR_FLAG			(0x00000010)	/**< Tag ID of EXIF 0x8769 */
#define FJ_EXIF_TAG_PRINTIM_FLAG			(0x00000020)	/**< Tag ID of EXIF 0xC4A5 */
#define FJ_EXIF_TAG_EXPTIME_FLAG			(0x00000040)	/**< Tag ID of EXIF 0x829A */
#define FJ_EXIF_TAG_FNUM_FLAG				(0x00000080)	/**< Tag ID of EXIF 0x829D */
#define FJ_EXIF_TAG_EXPPROG_FLAG			(0x00000100)	/**< Tag ID of EXIF 0x8822 */
#define FJ_EXIF_TAG_ISOSPEEDRATE_FLAG		(0x00000200)	/**< Tag ID of EXIF 0x8827 */
#define FJ_EXIF_TAG_DATETIMEORIG_FLAG		(0x00000400)	/**< Tag ID of EXIF 0x9003 */
#define FJ_EXIF_TAG_DATETIMEDITI_FLAG		(0x00000800)	/**< Tag ID of EXIF 0x9004 */
#define FJ_EXIF_TAG_COMPRESSBPP_FLAG		(0x00001000)	/**< Tag ID of EXIF 0x9102 */
#define FJ_EXIF_TAG_SHUTTERSPEED_FLAG		(0x00002000)	/**< Tag ID of EXIF 0x9201 */
#define FJ_EXIF_TAG_EXPBIAS_FLAG			(0x00004000)	/**< Tag ID of EXIF 0x9204 */
#define FJ_EXIF_TAG_METERMODE_FLAG			(0x00008000)	/**< Tag ID of EXIF 0x9207 */
#define FJ_EXIF_TAG_LIGHTSOURCE_FLAG		(0x00010000)	/**< Tag ID of EXIF 0x9208 */
#define FJ_EXIF_TAG_FLASH_FLAG				(0x00020000)	/**< Tag ID of EXIF 0x9209 */
#define FJ_EXIF_TAG_MAKERNOTE_FLAG			(0x00040000)	/**< Tag ID of EXIF 0x927C */
#define FJ_EXIF_TAG_PEXELXDIM_FLAG			(0x00080000)	/**< Tag ID of EXIF 0xA002 */
#define FJ_EXIF_TAG_PEXELYDIM_FLAG			(0x00100000)	/**< Tag ID of EXIF 0xA003 */
#define FJ_EXIF_TAG_CUSTOMRENDERED_FLAG		(0x00200000)	/**< Tag ID of EXIF 0xA401 */
#define FJ_EXIF_TAG_WB_FLAG					(0x00400000)	/**< Tag ID of EXIF 0xA403 */
#define FJ_EXIF_TAG_CONTRAST_FLAG			(0x00800000)	/**< Tag ID of EXIF 0xA408 */
#define FJ_EXIF_TAG_SATURATION_FLAG			(0x01000000)	/**< Tag ID of EXIF 0xA409 */
#define FJ_EXIF_TAG_MAKER1_FLAG				(0x02000000)	/**< Tag ID of EXIF 0x010F */
#define FJ_EXIF_TAG_MODEL1_FLAG				(0x04000000)	/**< Tag ID of EXIF 0x0110 */
#define FJ_EXIF_TAG_ORIENTTHM_FLAG			(0x08000000)	/**< Tag ID of EXIF 0x0112 */
#define FJ_EXIF_TAG_DATETIME1_FLAG			(0x10000000)	/**< Tag ID of EXIF 0x0132 */
#define FJ_EXIF_TAG_JPG_INTERCHGFMT_FLAG	(0x20000000)	/**< Tag ID of EXIF 0x0201 */
#define FJ_EXIF_TAG_JPG_INTERCHGFMTLEN_FLAG	(0x40000000)	/**< Tag ID of EXIF 0x0202 */

#define FJ_GPS_TAG_GPSVERSIONID_FLAG		(0x00000001)	/**< Tag ID of GPS-IFD 0x0000 (not supported) */
#define FJ_GPS_TAG_GPSLATITUDEREF_FLAG		(0x00000002)	/**< Tag ID of GPS-IFD 0x0001 */
#define FJ_GPS_TAG_GPSLATITUDE_FLAG			(0x00000004)	/**< Tag ID of GPS-IFD 0x0002 */
#define FJ_GPS_TAG_GPSLONGITUDEREF_FLAG		(0x00000008)	/**< Tag ID of GPS-IFD 0x0003 */
#define FJ_GPS_TAG_GPSLONGITUDE_FLAG		(0x00000010)	/**< Tag ID of GPS-IFD 0x0004 */
#define FJ_GPS_TAG_GPSALTITUDEREF_FLAG		(0x00000020)	/**< Tag ID of GPS-IFD 0x0005 (not supported) */
#define FJ_GPS_TAG_GPSALTITUDE_FLAG			(0x00000040)	/**< Tag ID of GPS-IFD 0x0006 (not supported) */
#define FJ_GPS_TAG_GPSTIMESTAMP_FLAG		(0x00000080)	/**< Tag ID of GPS-IFD 0x0007 (not supported) */
#define FJ_GPS_TAG_GPSSATELLITES_FLAG		(0x00000100)	/**< Tag ID of GPS-IFD 0x0008 (not supported) */
#define FJ_GPS_TAG_GPSSTATUS_FLAG			(0x00000200)	/**< Tag ID of GPS-IFD 0x0009 (not supported) */
#define FJ_GPS_TAG_GPSMEASUREMODE_FLAG		(0x00000400)	/**< Tag ID of GPS-IFD 0x000A (not supported) */
#define FJ_GPS_TAG_GPSDOP_FLAG				(0x00000800)	/**< Tag ID of GPS-IFD 0x000B (not supported) */
#define FJ_GPS_TAG_GPSSPEEDREF_FLAG			(0x00001000)	/**< Tag ID of GPS-IFD 0x000C (not supported) */
#define FJ_GPS_TAG_GPSSPEED_FLAG			(0x00002000)	/**< Tag ID of GPS-IFD 0x000D (not supported) */
#define FJ_GPS_TAG_GPSTRACKREF_FLAG			(0x00004000)	/**< Tag ID of GPS-IFD 0x000E (not supported) */
#define FJ_GPS_TAG_GPSTRACK_FLAG			(0x00008000)	/**< Tag ID of GPS-IFD 0x000F (not supported) */
#define FJ_GPS_TAG_GPSIMGDIRECTIONREF_FLAG	(0x00010000)	/**< Tag ID of GPS-IFD 0x0010 */
#define FJ_GPS_TAG_GPSIMGDIRECTION_FLAG		(0x00020000)	/**< Tag ID of GPS-IFD 0x0011 */
#define FJ_GPS_TAG_GPSMAPDATUM_FLAG			(0x00040000)	/**< Tag ID of GPS-IFD 0x0012 (not supported) */
#define FJ_GPS_TAG_GPSDESTLATITUDEREF_FLAG	(0x00080000)	/**< Tag ID of GPS-IFD 0x0013 (not supported) */
#define FJ_GPS_TAG_GPSDESTLATITUDE_FLAG		(0x00100000)	/**< Tag ID of GPS-IFD 0x0014 (not supported) */
#define FJ_GPS_TAG_GPSDESTLONGITUDEREF_FLAG	(0x00200000)	/**< Tag ID of GPS-IFD 0x0015 (not supported) */
#define FJ_GPS_TAG_GPSDESTLONGITUDE_FLAG	(0x00400000)	/**< Tag ID of GPS-IFD 0x0016 (not supported) */
#define FJ_GPS_TAG_GPSDESTBEARINGREF_FLAG	(0x00800000)	/**< Tag ID of GPS-IFD 0x0017 (not supported) */
#define FJ_GPS_TAG_GPSDESTBEARING_FLAG		(0x01000000)	/**< Tag ID of GPS-IFD 0x0018 (not supported) */
#define FJ_GPS_TAG_GPSDESTDISTANCEREF_FLAG	(0x02000000)	/**< Tag ID of GPS-IFD 0x0019 (not supported) */
#define FJ_GPS_TAG_GPSDESTDISTANCE_FLAG		(0x04000000)	/**< Tag ID of GPS-IFD 0x001A (not supported) */
#define FJ_GPS_TAG_GPSPROCESSINGMETHOD_FLAG	(0x08000000)	/**< Tag ID of GPS-IFD 0x001B (not supported) */
#define FJ_GPS_TAG_GPSAREAINFORMATION_FLAG	(0x10000000)	/**< Tag ID of GPS-IFD 0x001C (not supported) */
#define FJ_GPS_TAG_GPSDATESTAMP_FLAG		(0x20000000)	/**< Tag ID of GPS-IFD 0x001D (not supported) */
#define FJ_GPS_TAG_GPSDIFFERENTIAL_FLAG		(0x40000000)	/**< Tag ID of GPS-IFD 0x001E (not supported) */
#define FJ_GPS_TAG_GPSHPOSITIONERR_FLAG		(0x80000000)	/**< Tag ID of GPS-IFD 0x001F (not supported) */

///////////////////////////////////////////////////////////////////////////////
// Enumeration
///////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
/**
Description for FJ_EXIF_TYPE.
*/
typedef enum {
	FJ_TYPE_BYTE		= 1,		/**< Exif type BYTE */
	FJ_TYPE_ASCII,					/**< Exif type ASCII */
	FJ_TYPE_SHORT,					/**< Exif type SHORT */
	FJ_TYPE_LONG,					/**< Exif type LONG */
	FJ_TYPE_RATIONAL,				/**< Exif type RATIONAL */
	FJ_TYPE_SBYTE,					/**< Exif type Signed BYTE */
	FJ_TYPE_UNDEFINED,				/**< Exif type undefined */
	FJ_TYPE_SSHORT,					/**< Exif type Signed SHORT */
	FJ_TYPE_SLONG,					/**< Exif type Signed LONG */
	FJ_TYPE_SRATIONAL,				/**< Exif type Signed RATIONAL */
	FJ_TYPE_DELETE		= 0xFFFF	/**< Exif Tag delete */
} FJ_EXIF_TYPE;

/**
Description for FJ_EXIF_IFD.
*/
typedef enum {
	FJ_IFD_0,					/**< 0th IFD TIFF Tag */
	FJ_IFD_EXIF,				/**< 0th IFD Exif Private Tag */
	FJ_IFD_GPS,					/**< 0th IFD GPS Info Tag */
	FJ_IFD_INTEROPERABILITY,	/**< 0th IFD Interoperability Tag */
	FJ_IFD_1					/**< 1st IFD TIFF Tag */
} FJ_EXIF_IFD;

/*----------------------------------------------------------------------*/
/* Structure														    */
/*----------------------------------------------------------------------*/
/**
Description for T_EXIF_RATIONAL.
*/
typedef struct {
	ULONG Num;	/**< Numerator */
	ULONG Den;	/**< Denominator */
} T_EXIF_RATIONAL;

/**
Description for T_EXIF_SRATIONAL.
*/
typedef struct {
	LONG Num;	/**< Numerator */
	LONG Den;	/**< Denominator */
} T_EXIF_SRATIONAL;

///////////////////////////////////////////////////////////////////////////////
// external variable
///////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
// Nothing Special


/*@} --- end of fj_exif_definition */

///////////////////////////////////////////////////////////////////////////////
// macro
///////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
/** @weakgroup fj_exif_api
@{*/
// Nothing Special

///////////////////////////////////////////////////////////////////////////////
// API
///////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------*/
/* Function														  */
/*----------------------------------------------------------------*/
/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets EXIF tag parameter.
@param[in] IfdIndex The IFD index of EXIF
@param[in] TagId The Tag ID of EXIF
@param[in] Type The Type of EXIF tag
@param[in] Count The Count of EXIF tag
@param[in] SupportLevel The Suport Level of EXIF tag ( always 0 )
@param[in] Data The Value of specified EXIF tag
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
*/
FJ_ERR_CODE	FJ_SetImageEXIF(USHORT IfdIndex, USHORT TagId, USHORT Type, USHORT Count, USHORT SupportLevel, BYTE* Data);

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets EXIF tag parameter for MPF class1.
@param[in] IfdIndex The IFD index of EXIF
@param[in] TagId The Tag ID of EXIF
@param[in] Type The Type of EXIF tag
@param[in] Count The Count of EXIF tag
@param[in] SupportLevel The Suport Level of EXIF tag ( always 0 )
@param[in] Data The Value of specified EXIF tag
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
*/
FJ_ERR_CODE FJ_SetImageEXIF_Class1( USHORT IfdIndex, USHORT TagId, USHORT Type, USHORT Count, USHORT SupportLevel, BYTE* Data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function sets EXIF tag parameter for MPF class2.
@param[in] IfdIndex The IFD index of EXIF
@param[in] TagId The Tag ID of EXIF
@param[in] Type The Type of EXIF tag
@param[in] Count The Count of EXIF tag
@param[in] SupportLevel The Suport Level of EXIF tag ( always 0 )
@param[in] Data The Value of specified EXIF tag
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
*/
FJ_ERR_CODE FJ_SetImageEXIF_Class2( USHORT IfdIndex, USHORT TagId, USHORT Type, USHORT Count, USHORT SupportLevel, BYTE* Data );

/**
<table><tr><td>Task-independent portion</td><td>x</td></tr></table>
This function gets EXIF tag parameter.
@param[in] uwDcfIndex The index of DCF file
@param[in] uwIfdIndex The IFD index of EXIF
@param[in] uwTagId The Tag ID of EXIF
@param[out] puData The Value of specified EXIF tag
@param[out] pulCount The Count of EXIF tag
@retval FJ_ERR_OK : Success
@retval FJ_ERR_NG : Error
@remarks If this API return code is "FJ_ERR_NG", the argument "puData" will be "0".
*/
FJ_ERR_CODE	FJ_GetImageEXIF(LONG uwDcfIndex, USHORT uwIfdIndex, USHORT uwTagId, BYTE* puData, USHORT* pulCount);

/*@} --- end of fj_dcf_api */

///////////////////////////////////////////////////////////////////////////////
// message
///////////////////////////////////////////////////////////////////////////////
#endif	// _FJ_EXIF_H
