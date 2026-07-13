/**
 * @file		exif.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

#include <string.h>
#include "fj_exif.h"
#include "dcf_if.h"
#include "exif_controller.h"
#include "sdram_map_common.h"
#include "fs_user_func.h"

/*---------------------------------------------------------------*/
/* Definition													 */
/*---------------------------------------------------------------*/
#define CO_BF_EXIF_DIRECT

/*---------------------------------------------------------------*/
/* Enumeration													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Structure													 */
/*---------------------------------------------------------------*/
// Nothing Special

/*---------------------------------------------------------------*/
/* Global Data													 */
/*---------------------------------------------------------------*/
static volatile CHAR* g_bf_exif_param_buff;
static volatile LONG g_bf_exif_index_buf = 0xFFFFFFFF;

/*----------------------------------------------------------------*/
/* Macro														  */
/*----------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------*/
/* Local Function												  */
/*----------------------------------------------------------------*/

#ifdef FJ_DEBUG_PRINT_ON
VOID exif_set_value(INT32 type, INT32 count, LONG input1, LONG input2)
{
	SHORT s_value;
	LONG l_value;
	T_BF_DCF_EXIF_RATIONAL rational_value;

	if (type == 0) {
		s_value = (SHORT)input1;
		memcpy((FW_VOID*)g_bf_exif_param_buff, &s_value, count);
	}
	else if(type == 1) {
		l_value = (ULONG)input1;
		memcpy((FW_VOID*)g_bf_exif_param_buff, &l_value, count);
	}
	else if (type == 2) {
		rational_value.Num = input1;
		rational_value.Den = input2;
		memcpy((FW_VOID*)g_bf_exif_param_buff, &rational_value, count);
	}
	else{
		; // DO NOTHING
	}
}
#endif // FJ_DEBUG_PRINT_ON

/*----------------------------------------------------------------*/
/* Global Function												  */
/*----------------------------------------------------------------*/

VOID BF_Exif_Reset_Index(VOID)
{
	g_bf_exif_index_buf = 0xFFFFFFFF;
}

#if 0
/**
 * @brief		allocate a new memory area for exif
 * @param		FJ_MEMORY_LOCATION	Location
 *				INT32				Buffer
 *				INT32				Size
 * @return		Success/Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Exif_Set_Mem_Location_Ptr( FJ_MEMORY_LOCATION Location, CHAR** Buffer, INT32 Size )
{
	INT32 ercd;
	FW_VOID*	 buf;

	switch( Location ){
		case FJ_EXIF_MEM_LOCATION:
			if( g_bf_exif_param_buff != NULL ){
				BF_Fs_User_Custom_Rel_Mpl( MPKID_EXIF, (FW_VOID*)g_bf_exif_param_buff );
				g_bf_exif_param_buff = NULL;
			}

			ercd = BF_Fs_User_Custom_Get_Mpl( MPKID_EXIF, Size, &buf );
			if( ercd != D_BF_FS_USER_CUSTOM_OK ){
				return FJ_ERR_NG;
			}

			g_bf_exif_param_buff = buf;
			*Buffer = buf;
			break;

		default:
			// DO NOTHING
			break;
	}
	return FJ_ERR_OK;
}

/**
 * @brief		release a memory area for exif
 * @param		FJ_MEMORY_LOCATION	Location
 * @return		Success/Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Exif_Unset_Mem_Location_Ptr( FJ_MEMORY_LOCATION Location )
{
	switch( Location ){
		case FJ_EXIF_MEM_LOCATION:
			if( g_bf_exif_param_buff != NULL ){
				BF_Fs_User_Custom_Rel_Mpl( MPKID_EXIF, (FW_VOID*)g_bf_exif_param_buff );
				g_bf_exif_param_buff = NULL;
			}
			break;

		default:
			// DO NOTHING
			break;
	}
	return FJ_ERR_OK;
}
#endif


/**
 * @brief		read EXIF header
 * @param		INT32	uwDcfIndex
 *				INT32	uwIfdIndex
 *				INT32	uwTagID
 *				INT32	pulCount
 * @return		Success/Fail
 * @note		None
 * @attention	None
 */
#ifdef CO_BF_EXIF_DIRECT
FJ_ERR_CODE fj_getimageexif( LONG uwDcfIndex, USHORT uwIfdIndex, USHORT uwTagId, BYTE* puData, USHORT* pulCount )
{
	static T_BF_DCF_IF_IMAGE_DETAIL img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA img_data;
	SHORT rotate;

	if( uwDcfIndex != g_bf_exif_index_buf ){
		/* get current exif info for jpeg */
		if( BF_Dcf_If_Get_Exif_Info_Jpeg(uwDcfIndex, &img_detail, &img_data) != 0 ){
//			puData = 0;
			*puData = 0x00;
			g_bf_exif_index_buf = 0xFFFFFFFF;
			BF_Debug_Print_Error(("BF_Dcf_If_Get_Exif_Info_Jpeg err\n"));
			return FJ_ERR_NG;
		}
		g_bf_exif_index_buf = uwDcfIndex;
	}

	// get EXIF header
	if( uwIfdIndex == FJ_IFD_0){
		switch( uwTagId ){
			case FJ_IFD0_TAG_MAKE_:  // Make
				*pulCount = sizeof(DCF_ASCII) * D_BF_DCF_EXIF_V_MAKE_S;
				//memcpy( puData, img_detail.maker, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_MAKE_FLAG)) {
					memcpy( puData, img_detail.maker, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_IFD0_TAG_MODEL_:  // Model
				*pulCount = sizeof(DCF_ASCII) * D_BF_DCF_EXIF_V_MODEL_S;
				//memcpy( puData, img_detail.model, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_MODEL_FLAG)) {
					memcpy( puData, img_detail.model, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_IFD0_TAG_ORIENT_:  // Orientation
				*pulCount = sizeof(SHORT);
				//rotate = img_detail.rotate;
				//memcpy( puData, &rotate, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_ORIENT_FLAG)) {
					rotate = img_detail.rotate;
					memcpy( puData, &rotate, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_IFD0_TAG_XRES_:  // XResolution
				*pulCount = sizeof(USHORT);
				*((USHORT *)puData) = (USHORT)img_detail.width;
				break;

			case FJ_IFD0_TAG_YRES_:  // YResolution
				*pulCount = sizeof(USHORT);
				*((USHORT *)puData) = (USHORT)img_detail.lines;
				break;

			case FJ_IFD0_TAG_RESUNIT_:  // ResolutionUnit
//				*pulCount = sizeof(SHORT) * gJpeg_Header.Ifd0.ResolutionUnit.Count;
//				*puData = gJpeg_Header.Ifd0.ResolutionUnit.ValueOffset.SValue.Value;
				break;

			case FJ_IFD0_TAG_SOFT_:  // Software
//				*pulCount = sizeof(DCF_ASCII) * gJpeg_Header.Ifd0.Software.Count;
//				memcpy( puData, gJpeg_Header.Ifd0.ValueSoftware, *pulCount );
				break;

			case FJ_IFD0_TAG_DATETIME_:  // DateTime
				*pulCount = sizeof(DCF_ASCII) * 20;
//				sprintf( (char *)&puData[0], "%04d", img_detail.DateTimeOriginal.ad_year );
//				puData[4] = ':';
//				sprintf( (char *)&puData[5], "%02d", img_detail.DateTimeOriginal.month );
//				puData[7] = ':';
//				sprintf( (char *)&puData[8], "%02d", img_detail.DateTimeOriginal.day );
//				puData[10] = ' ';
//				sprintf( (char *)&puData[11], "%02d", img_detail.DateTimeOriginal.hour );
//				puData[13] = ':';
//				sprintf( (char *)&puData[14], "%02d", img_detail.DateTimeOriginal.min );
//				puData[16] = ':';
//				sprintf( (char *)&puData[17], "%02d", img_detail.DateTimeOriginal.sec );
//				puData[19] = '\0';
//				memcpy( puData, (void*)img_detail.edit_date_time.ad_year, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_DATETIME_FLAG)) {
					sprintf( (char*)&puData[0], "%04d", img_detail.DateTimeOriginal.ad_year );
					puData[4] = ':';
					sprintf( (char*)&puData[5], "%02d", img_detail.DateTimeOriginal.month );
					puData[7] = ':';
					sprintf( (char*)&puData[8], "%02d", img_detail.DateTimeOriginal.day );
					puData[10] = ' ';
					sprintf( (char*)&puData[11], "%02d", img_detail.DateTimeOriginal.hour );
					puData[13] = ':';
					sprintf( (char*)&puData[14], "%02d", img_detail.DateTimeOriginal.min );
					puData[16] = ':';
					sprintf( (char*)&puData[17], "%02d", img_detail.DateTimeOriginal.sec );
					puData[19] = '\0';
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_IFD0_TAG_YCBCRPOS_:  //YCbCrPositioning
//				*pulCount = sizeof(DCF_ASCII) * gJpeg_Header.Ifd0.YCbCrPositioning.Count;
//				*puData = gJpeg_Header.Ifd0.YCbCrPositioning.ValueOffset.SValue.Value;
				break;

			case FJ_IFD0_TAG_COPYRIGHT_:  // Copyright
//				*pulCount = sizeof(DCF_ASCII) * gJpeg_Header.Ifd0.Copyright.Count;
//				memcpy( puData, gJpeg_Header.Ifd0.ValueCopyright, *pulCount );
				break;

			case FJ_IFD0_TAG_EXIFPTR_:  // ExifIFDPointer
//				*pulCount = sizeof(LONG) * gJpeg_Header.Ifd0.ExifIFDPointer.Count;
//				*puData = gJpeg_Header.Ifd0.ExifIFDPointer.ValueOffset.LValue;
				break;

			default:
				return FJ_ERR_NG;
		}
	}
	else if( uwIfdIndex == FJ_IFD_EXIF ){
		switch( uwTagId ){
			case FJ_EXIF_TAG_EXPTIME_:  // ExposureTime
				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL);
				//memcpy( puData, &img_detail.EpTimeNum, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_EXPTIME_FLAG)) {
					memcpy( puData, &img_detail.EpTimeNum, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_FNUM_:  // FNumber
				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL);
				//memcpy( puData, &img_detail.FvNum, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_FNUM_FLAG)) {
					memcpy( puData, &img_detail.FvNum, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_EXPPROG_:  // ExposureProgram
				*pulCount = sizeof(SHORT);
				//memcpy( puData, &img_detail.EpProgram, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_EXPPROG_FLAG)) {
					memcpy( puData, &img_detail.EpProgram, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_ISOSPEEDRATE_:  // ISOSpeedRatings
				*pulCount = sizeof(SHORT);
				//memcpy( puData, &img_detail.iso, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_ISOSPEEDRATE_FLAG)) {
					memcpy( puData, &img_detail.iso, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_EXIFVER_:  //ExifVersion
//				*pulCount = gJpeg_Header.ExifIfd.ExifVersion.Count;
//				*puData = gJpeg_Header.ExifIfd.ExifVersion.ValueOffset.SValue.Value;
				break;

			case FJ_EXIF_TAG_DATETIMEORIG_:  // DateTimeOriginal
				*pulCount = sizeof(DCF_ASCII) * 20;
//				sprintf( (char *)&puData[0], "%04d", img_detail.DateTimeOriginal.ad_year );
//				puData[4] = ':';
//				sprintf( (char *)&puData[5], "%02d", img_detail.DateTimeOriginal.month );
//				puData[7] = ':';
//				sprintf( (char *)&puData[8], "%02d", img_detail.DateTimeOriginal.day );
//				puData[10] = ' ';
//				sprintf( (char *)&puData[11], "%02d", img_detail.DateTimeOriginal.hour );
//				puData[13] = ':';
//				sprintf( (char *)&puData[14], "%02d", img_detail.DateTimeOriginal.min );
//				puData[16] = ':';
//				sprintf( (char *)&puData[17], "%02d", img_detail.DateTimeOriginal.sec );
//				puData[19] = '\0';
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_DATETIMEORIG_FLAG)) {
					sprintf( (char*)&puData[0], "%04d", img_detail.DateTimeOriginal.ad_year );
					puData[4] = ':';
					sprintf( (char*)&puData[5], "%02d", img_detail.DateTimeOriginal.month );
					puData[7] = ':';
					sprintf( (char*)&puData[8], "%02d", img_detail.DateTimeOriginal.day );
					puData[10] = ' ';
					sprintf( (char*)&puData[11], "%02d", img_detail.DateTimeOriginal.hour );
					puData[13] = ':';
					sprintf( (char*)&puData[14], "%02d", img_detail.DateTimeOriginal.min );
					puData[16] = ':';
					sprintf( (char*)&puData[17], "%02d", img_detail.DateTimeOriginal.sec );
					puData[19] = '\0';
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_DATETIMEDITI_:  // DateTimeDigitized
				*pulCount = sizeof(DCF_ASCII) * 20;
//				sprintf( (char *)&puData[0], "%04d", img_detail.DateTimeDigital.ad_year );
//				puData[4] = ':';
//				sprintf( (char *)&puData[5], "%02d", img_detail.DateTimeDigital.month );
//				puData[7] = ':';
//				sprintf( (char *)&puData[8], "%02d", img_detail.DateTimeDigital.day );
//				puData[10] = ' ';
//				sprintf( (char *)&puData[11], "%02d", img_detail.DateTimeDigital.hour );
//				puData[13] = ':';
//				sprintf( (char *)&puData[14], "%02d", img_detail.DateTimeDigital.min );
//				puData[16] = ':';
//				sprintf( (char *)&puData[17], "%02d", img_detail.DateTimeDigital.sec );
//				puData[19] = '\0';
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_DATETIMEDITI_FLAG)) {
					sprintf( (char*)&puData[0], "%04d", img_detail.DateTimeDigital.ad_year );
					puData[4] = ':';
					sprintf( (char*)&puData[5], "%02d", img_detail.DateTimeDigital.month );
					puData[7] = ':';
					sprintf( (char*)&puData[8], "%02d", img_detail.DateTimeDigital.day );
					puData[10] = ' ';
					sprintf( (char*)&puData[11], "%02d", img_detail.DateTimeDigital.hour );
					puData[13] = ':';
					sprintf( (char*)&puData[14], "%02d", img_detail.DateTimeDigital.min );
					puData[16] = ':';
					sprintf( (char*)&puData[17], "%02d", img_detail.DateTimeDigital.sec );
					puData[19] = '\0';
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_COMPCONFIGURE_:  // ComponentsConfiguration
//				*pulCount = gJpeg_Header.ExifIfd.ComponentsConfiguration.Count;
//				memcpy( puData, &gJpeg_Header.ExifIfd.ComponentsConfiguration.ValueOffset.LValue, *pulCount );
				break;

			case FJ_EXIF_TAG_COMPRESSBPP_:  // CompressedBitsPerPixel
				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL);
				//memcpy( puData, &img_detail.bitperpixelNum, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_COMPRESSBPP_FLAG)) {
					memcpy( puData, &img_detail.bitperpixelNum, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_SHUTTER_:  // ShutterSpeedValue
				*pulCount = sizeof(T_BF_DCF_EXIF_SRATIONAL);
//				memcpy( puData, &gJpeg_Header.ExifIfd.ValueShutterSpeedValue, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_SHUTTERSPEED_FLAG)) {
					memcpy( puData, &img_detail.ShutterSpeedNum, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_APERTURE_:  // ApertureValue
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.ExifIfd.ApertureValue.Count;
//				memcpy( puData, &gJpeg_Header.ExifIfd.ValueApertureValue, *pulCount );
				break;

			case FJ_EXIF_TAG_EXPBIAS_:  // ExposureBiasValue
				*pulCount = sizeof(T_BF_DCF_EXIF_SRATIONAL);
				//memcpy( puData, &img_detail.ExposureBiasNum, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_EXPBIAS_FLAG)) {
					memcpy( puData, &img_detail.ExposureBiasNum, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_MAXAPTURE_:  //MaxApertureValue
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.ExifIfd.MaxApertureValue.Count;
//				memcpy( puData, &gJpeg_Header.ExifIfd.ValueMaxApertureValue, *pulCount );
				break;

			case FJ_EXIF_TAG_METERMODE_:  // MeteringMode
				*pulCount = sizeof(SHORT);
				//memcpy( puData, &img_detail.MeteringMode, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_METERMODE_FLAG)) {
					memcpy( puData, &img_detail.MeteringMode, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_LIGHTSOURCE_:  // LightSource
				*pulCount = sizeof(SHORT);
				//memcpy( puData, &img_detail.LightSource, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_LIGHTSOURCE_FLAG)) {
					memcpy( puData, &img_detail.LightSource, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_FLASH_:  // Flash
				*pulCount = sizeof(SHORT);
				//memcpy( puData, &img_detail.flash, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_FLASH_FLAG)) {
					memcpy( puData, &img_detail.flash, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_FOCALLEN_:  // FocalLength
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.ExifIfd.FocalLength.Count;
//				*puData = gJpeg_Header.ExifIfd.FocalLength.ValueOffset.SValue.Value;
				break;

			case FJ_EXIF_TAG_MAKERNOTE_:  // MakerNote
				*pulCount = img_detail.maker_note_length;
				//memcpy( puData, &img_detail.maker_note_offset, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_MAKERNOTE_FLAG)) {
					memcpy( puData, &img_detail.maker_note_offset, sizeof(img_detail.maker_note_offset) );
				}
				else {
					memset( puData, 0x00, sizeof(img_detail.maker_note_offset) );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_FLRESHPIXVER_:  // FlashPixVersion
//				*pulCount = gJpeg_Header.ExifIfd.FlashPixVersion.Count;
//				memcpy( puData, &gJpeg_Header.ExifIfd.FlashPixVersion.ValueOffset.LValue, *pulCount );
				break;

			case FJ_EXIF_TAG_COLORSPACE_:  // ColorSpace
//				*pulCount = sizeof(SHORT) * gJpeg_Header.ExifIfd.ColorSpace.Count;
//				*puData = gJpeg_Header.ExifIfd.ColorSpace.ValueOffset.SValue.Value;
				break;

			case FJ_EXIF_TAG_PEXELXDIM_:  // PixelXDimension
				*pulCount = sizeof(DCF_LONG);
				//memcpy( puData, &img_detail.width, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_PEXELXDIM_FLAG)) {
					memcpy( puData, &img_detail.width, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_PEXELYDIM_:  // PixelYDimension
				*pulCount = sizeof(DCF_LONG);
				//memcpy( puData, &img_detail.lines, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_PEXELYDIM_FLAG)) {
					memcpy( puData, &img_detail.lines, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_ITOPIFDPTR_:  // InteroperabilityIFDPointer
//				*pulCount = sizeof(LONG) * gJpeg_Header.ExifIfd.InteroperabilityIFDPointer.Count;
//				*puData = gJpeg_Header.ExifIfd.InteroperabilityIFDPointer.ValueOffset.LValue;
				break;

			case 0xA215:  // ExposureIndex
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.ExifIfd.ExposureIndex.Count;
//				memcpy( puData, &gJpeg_Header.ExifIfd.ValueExposureBiasValue, *pulCount );
				break;

			case FJ_EXIF_TAG_SENSOR_:  // SensingMethod
//				*pulCount = sizeof(SHORT) * gJpeg_Header.ExifIfd.SensingMethod.Count;
//				*puData = gJpeg_Header.ExifIfd.SensingMethod.ValueOffset.SValue.Value;
				break;

			case FJ_EXIF_TAG_FILESOURCE_:  // FileSource
//				*pulCount = gJpeg_Header.ExifIfd.FileSource.Count;
//				memcpy( puData, &gJpeg_Header.ExifIfd.FileSource.ValueOffset.LValue, *pulCount );
				break;

			case FJ_EXIF_TAG_SENCETYPE_:  // SceneType
//				*pulCount = gJpeg_Header.ExifIfd.SceneType.Count;
//				memcpy( puData, &gJpeg_Header.ExifIfd.SceneType.ValueOffset.LValue, *pulCount );
				break;

			case FJ_EXIF_TAG_EXPOSUREMODE_:  // ExposureMode
//				*pulCount = sizeof(SHORT) * gJpeg_Header.ExifIfd.ExposureMode.Count;
//				*puData = gJpeg_Header.ExifIfd.ExposureMode.ValueOffset.SValue.Value;
				break;

			case FJ_EXIF_TAG_WB_:  // WhiteBalanceMode
				*pulCount = sizeof(SHORT);
				//memcpy( puData, &img_detail.WBMode, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_WB_FLAG)) {
					memcpy( puData, &img_detail.WBMode, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_EXIF_TAG_DZOOM_:  // DigitalZoomRatio
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.ExifIfd.DigitalZoomRatio.Count;
//				memcpy( puData, &gJpeg_Header.ExifIfd.ValueDigitalZoomRatio, *pulCount );
				break;

			case FJ_EXIF_TAG_SHARPNESS_:  // Sharpness
//				*pulCount = sizeof(SHORT) * gJpeg_Header.ExifIfd.Sharpness.Count;
//				*puData = gJpeg_Header.ExifIfd.Sharpness.ValueOffset.SValue.Value;
				break;

			default:
				return FJ_ERR_NG;
		}
	}
	else if( uwIfdIndex == FJ_IFD_INTEROPERABILITY ){
		switch( uwTagId ){
			case FJ_ITOP_TAG_ITOPINDEX_:  // InteroperabilityIndex
//				*pulCount = sizeof(DCF_ASCII) * gJpeg_Header.R98Ifd.InteroperabilityIndex.Count;
//				memcpy( puData, &gJpeg_Header.R98Ifd.InteroperabilityIndex.ValueOffset.SValue.Value, *pulCount );
				break;

			case FJ_ITOP_TAG_ITOPVERSION_:  // InteroperabilityVersion
//				*pulCount = gJpeg_Header.R98Ifd.InteroperabilityVersion.Count;
//				memcpy( puData, &gJpeg_Header.R98Ifd.InteroperabilityVersion.ValueOffset.LValue, *pulCount );
				break;

			default:
				return FJ_ERR_NG;
		}
	}
#ifdef CO_BF_DCF_IF_GET_GPSTAG
	else if (uwIfdIndex == FJ_IFD_GPS) {
		switch (uwTagId) {
			case FJ_GPS_TAG_GPSLATITUDEREF_:	// GPSLatitudeRef
				*pulCount = sizeof(DCF_ASCII) * 2;
				if (0 != (img_detail.gpstag_flg & FJ_GPS_TAG_GPSLATITUDEREF_FLAG)) {
					memcpy(puData, img_detail.gpsLatitudeRef, *pulCount);
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_GPS_TAG_GPSLATITUDE_:	// GPSLatitude
				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * 3;
				if (0 != (img_detail.gpstag_flg & FJ_GPS_TAG_GPSLATITUDE_FLAG)) {
					memcpy(puData, &img_detail.gpsLatitudeDdNum, *pulCount);
				}
				else {
					memset(puData, 0x00, *pulCount);
					return FJ_ERR_NG;
				}
				break;

			case FJ_GPS_TAG_GPSLONGITUDEREF_:	// GPSLongitudeRef
				*pulCount = sizeof(DCF_ASCII) * 2;
				if (0 != (img_detail.gpstag_flg & FJ_GPS_TAG_GPSLONGITUDEREF_FLAG)) {
					memcpy(puData, img_detail.gpsLongitudeRef, *pulCount);
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_GPS_TAG_GPSLONGITUDE_:	// GPSLongitude
				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * 3;
				if (0 != (img_detail.gpstag_flg & FJ_GPS_TAG_GPSLONGITUDE_FLAG)) {
					memcpy(puData, &img_detail.gpsLongitudeDdNum, *pulCount);
				}
				else {
					memset(puData, 0x00, *pulCount);
					return FJ_ERR_NG;
				}
				break;

			case FJ_GPS_TAG_GPSIMGDIRECTIONREF_:	// GPSImgDirectionRef
				*pulCount = sizeof(DCF_ASCII) * 2;
				if (0 != (img_detail.gpstag_flg & FJ_GPS_TAG_GPSIMGDIRECTIONREF_FLAG)) {
					memcpy(puData, img_detail.gpsImgDirectionRef, *pulCount);
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_GPS_TAG_GPSIMGDIRECTION_:	// GPSImgDirection
				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL);
				if (0 != (img_detail.gpstag_flg & FJ_GPS_TAG_GPSIMGDIRECTION_FLAG)) {
					memcpy(puData, &img_detail.gpsImgDirectionNum, *pulCount);
				}
				else {
					memset(puData, 0x00, *pulCount);
					return FJ_ERR_NG;
				}
				break;

			default:
				return FJ_ERR_NG;
		}
	}
#endif	// CO_BF_DCF_IF_GET_GPSTAG
	else if( uwIfdIndex == FJ_IFD_1 ){
		switch( uwTagId ){
			case FJ_IFD1_TAG_COMPRESS_:  // Compression
//				*pulCount = sizeof(SHORT) * gJpeg_Header.Ifd1.Compression.Count;
//				*puData = gJpeg_Header.Ifd1.Compression.ValueOffset.SValue.Value;
				break;

			case FJ_IFD1_TAG_ORIENT_:  // Orientation
				*pulCount = sizeof(SHORT);
				//rotate = img_detail.rotate;
				//memcpy( puData, &rotate, *pulCount );
				if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_ORIENTTHM_FLAG)) {
					rotate = img_detail.rotate_thumb;
					memcpy( puData, &rotate, *pulCount );
				}
				else {
					memset( puData, 0x00, *pulCount );
					return FJ_ERR_NG;
				}
				break;

			case FJ_IFD1_TAG_XRES_:  // XResolution
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.Ifd1.XResolution.Count;
//				memcpy( puData, &gJpeg_Header.Ifd1.ValueXResolution, *pulCount );
				break;

			case FJ_IFD1_TAG_YRES_:  // YResolution
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.Ifd1.YResolution.Count;
//				memcpy( puData, &gJpeg_Header.Ifd1.ValueYResolution, *pulCount );
				break;

			case FJ_IFD1_TAG_RESUINT_:  // ResolutionUnit
//				*pulCount = sizeof(SHORT) * gJpeg_Header.Ifd1.ResolutionUnit.Count;
//				*puData = gJpeg_Header.Ifd1.ResolutionUnit.ValueOffset.SValue.Value;
				break;

			case FJ_IFD1_TAG_JPG_INTERCHGFMT_:  // JPEGInterchangeFormat
//				*pulCount = sizeof(LONG) * gJpeg_Header.Ifd1.JPEGInterchangeFormat.Count;
//				*puData = gJpeg_Header.Ifd1.JPEGInterchangeFormat.ValueOffset.LValue;
				break;

			case FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_:  //JPEGInterchangeFormatLength
//				*pulCount = sizeof(LONG) * gJpeg_Header.Ifd1.JPEGInterchangeFormatLength.Count;
//				*puData = gJpeg_Header.Ifd1.JPEGInterchangeFormatLength.ValueOffset.LValue;
				break;

			default:
				return FJ_ERR_NG;
		}
	}
	else{
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;

}

#else
FJ_ERR_CODE fj_getimageexif( USHORT uwDcfIndex, USHORT uwIfdIndex, USHORT uwTagId, USHORT* pulCount )
{
	static T_BF_DCF_IF_IMAGE_DETAIL img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA img_data;

	if( uwDcfIndex != g_bf_exif_index_buf ){
		BF_Dcf_If_Get_Exif_Info_Jpeg(uwDcfIndex, &img_detail, &img_data);
		g_bf_exif_index_buf = uwDcfIndex;
	}

	// get EXIF header
	if( uwIfdIndex == FJ_IFD_0){
		switch( uwTagId ){
			case FJ_IFD0_TAG_MAKE_:  // Make
				*pulCount = sizeof(DCF_ASCII) * D_BF_DCF_EXIF_V_MAKE_S;
				memcpy( g_bf_exif_param_buff, img_detail.maker, *pulCount );
				break;

			case FJ_IFD0_TAG_MODEL_:  // Model
				*pulCount = sizeof(DCF_ASCII) * D_BF_DCF_EXIF_V_MODEL_S;
				memcpy( g_bf_exif_param_buff, img_detail.model, *pulCount );
				break;

			case FJ_IFD0_TAG_ORIENT_:  // Orientation
				*pulCount = sizeof(SHORT);
				memcpy( g_bf_exif_param_buff, &img_detail.rotate, *pulCount );
				break;

			case FJ_IFD0_TAG_XRES_:  // XResolution
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.Ifd0.XResolution.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.Ifd0.ValueXResolution, *pulCount );
				break;

			case FJ_IFD0_TAG_YRES_:  // YResolution
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.Ifd0.YResolution.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.Ifd0.ValueYResolution, *pulCount );
				break;

			case FJ_IFD0_TAG_RESUNIT_:  // ResolutionUnit
//				*pulCount = sizeof(SHORT) * gJpeg_Header.Ifd0.ResolutionUnit.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.Ifd0.ResolutionUnit.ValueOffset.SValue.Value;
				break;

			case FJ_IFD0_TAG_SOFT_:  // Software
//				*pulCount = sizeof(DCF_ASCII) * gJpeg_Header.Ifd0.Software.Count;
//				memcpy( g_bf_exif_param_buff, gJpeg_Header.Ifd0.ValueSoftware, *pulCount );
				break;

			case FJ_IFD0_TAG_DATETIME_:  // DateTime
				*pulCount = sizeof(DCF_ASCII) * 20;
				sprintf( &g_bf_exif_param_buff[0], "%04d", img_detail.DateTimeOriginal.ad_year );
				g_bf_exif_param_buff[4] = ':';
				sprintf( &g_bf_exif_param_buff[5], "%02d", img_detail.DateTimeOriginal.month );
				g_bf_exif_param_buff[7] = ':';
				sprintf( &g_bf_exif_param_buff[8], "%02d", img_detail.DateTimeOriginal.day );
				g_bf_exif_param_buff[10] = ' ';
				sprintf( &g_bf_exif_param_buff[11], "%02d", img_detail.DateTimeOriginal.hour );
				g_bf_exif_param_buff[13] = ':';
				sprintf( &g_bf_exif_param_buff[14], "%02d", img_detail.DateTimeOriginal.min );
				g_bf_exif_param_buff[16] = ':';
				sprintf( &g_bf_exif_param_buff[17], "%02d", img_detail.DateTimeOriginal.sec );
				g_bf_exif_param_buff[19] = '\0';
//				memcpy( g_bf_exif_param_buff, (void*)img_detail.edit_date_time.ad_year, *pulCount );
				break;

			case FJ_IFD0_TAG_YCBCRPOS_:  //YCbCrPositioning
//				*pulCount = sizeof(DCF_ASCII) * gJpeg_Header.Ifd0.YCbCrPositioning.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.Ifd0.YCbCrPositioning.ValueOffset.SValue.Value;
				break;

			case FJ_IFD0_TAG_COPYRIGHT_:  // Copyright
//				*pulCount = sizeof(DCF_ASCII) * gJpeg_Header.Ifd0.Copyright.Count;
//				memcpy( g_bf_exif_param_buff, gJpeg_Header.Ifd0.ValueCopyright, *pulCount );
				break;

			case FJ_IFD0_TAG_EXIFPTR_:  // ExifIFDPointer
//				*pulCount = sizeof(LONG) * gJpeg_Header.Ifd0.ExifIFDPointer.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.Ifd0.ExifIFDPointer.ValueOffset.LValue;
				break;

			default:
				return FJ_ERR_NG;
		}
	}
	else if( uwIfdIndex == FJ_IFD_EXIF ){
		switch( uwTagId ){
			case FJ_EXIF_TAG_EXPTIME_:  // ExposureTime
				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL);
				memcpy( g_bf_exif_param_buff, &img_detail.EpTimeNum, *pulCount );
				break;

			case FJ_EXIF_TAG_FNUM_:  // FNumber
				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL);
				memcpy( g_bf_exif_param_buff, &img_detail.FvNum, *pulCount );
				break;

			case FJ_EXIF_TAG_EXPPROG_:  // ExposureProgram
				*pulCount = sizeof(SHORT);
				memcpy( g_bf_exif_param_buff, &img_detail.EpProgram, *pulCount );
				break;

			case FJ_EXIF_TAG_ISOSPEEDRATE_:  // ISOSpeedRatings
				*pulCount = sizeof(SHORT);
				memcpy( g_bf_exif_param_buff, &img_detail.iso, *pulCount );
				break;

			case FJ_EXIF_TAG_EXIFVER_:  //ExifVersion
//				*pulCount = gJpeg_Header.ExifIfd.ExifVersion.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.ExifIfd.ExifVersion.ValueOffset.SValue.Value;
				break;

			case FJ_EXIF_TAG_DATETIMEORIG_:  // DateTimeOriginal
				*pulCount = sizeof(DCF_ASCII) * 20;
				sprintf( &g_bf_exif_param_buff[0], "%04d", img_detail.DateTimeOriginal.ad_year );
				g_bf_exif_param_buff[4] = ':';
				sprintf( &g_bf_exif_param_buff[5], "%02d", img_detail.DateTimeOriginal.month );
				g_bf_exif_param_buff[7] = ':';
				sprintf( &g_bf_exif_param_buff[8], "%02d", img_detail.DateTimeOriginal.day );
				g_bf_exif_param_buff[10] = ' ';
				sprintf( &g_bf_exif_param_buff[11], "%02d", img_detail.DateTimeOriginal.hour );
				g_bf_exif_param_buff[13] = ':';
				sprintf( &g_bf_exif_param_buff[14], "%02d", img_detail.DateTimeOriginal.min );
				g_bf_exif_param_buff[16] = ':';
				sprintf( &g_bf_exif_param_buff[17], "%02d", img_detail.DateTimeOriginal.sec );
				g_bf_exif_param_buff[19] = '\0';
				break;

			case FJ_EXIF_TAG_DATETIMEDITI_:  // DateTimeDigitized
				*pulCount = sizeof(DCF_ASCII) * 20;
				sprintf( &g_bf_exif_param_buff[0], "%04d", img_detail.DateTimeDigital.ad_year );
				g_bf_exif_param_buff[4] = ':';
				sprintf( &g_bf_exif_param_buff[5], "%02d", img_detail.DateTimeDigital.month );
				g_bf_exif_param_buff[7] = ':';
				sprintf( &g_bf_exif_param_buff[8], "%02d", img_detail.DateTimeDigital.day );
				g_bf_exif_param_buff[10] = ' ';
				sprintf( &g_bf_exif_param_buff[11], "%02d", img_detail.DateTimeDigital.hour );
				g_bf_exif_param_buff[13] = ':';
				sprintf( &g_bf_exif_param_buff[14], "%02d", img_detail.DateTimeDigital.min );
				g_bf_exif_param_buff[16] = ':';
				sprintf( &g_bf_exif_param_buff[17], "%02d", img_detail.DateTimeDigital.sec );
				g_bf_exif_param_buff[19] = '\0';
				break;

			case FJ_EXIF_TAG_COMPCONFIGURE_:  // ComponentsConfiguration
//				*pulCount = gJpeg_Header.ExifIfd.ComponentsConfiguration.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.ExifIfd.ComponentsConfiguration.ValueOffset.LValue, *pulCount );
				break;

			case FJ_EXIF_TAG_COMPRESSBPP_:  // CompressedBitsPerPixel
				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL);
				memcpy( g_bf_exif_param_buff, &img_detail.bitperpixelNum, *pulCount );
				break;

			case FJ_EXIF_TAG_SHUTTER_:  // ShutterSpeedValue
//				*pulCount = sizeof(T_BF_DCF_EXIF_SRATIONAL) * gJpeg_Header.ExifIfd.ShutterSpeedValue.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.ExifIfd.ValueShutterSpeedValue, *pulCount );
				break;

			case FJ_EXIF_TAG_APERTURE_:  // ApertureValue
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.ExifIfd.ApertureValue.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.ExifIfd.ValueApertureValue, *pulCount );
				break;

			case FJ_EXIF_TAG_EXPBIAS_:  // ExposureBiasValue
				*pulCount = sizeof(T_BF_DCF_EXIF_SRATIONAL);
				memcpy( g_bf_exif_param_buff, &img_detail.ExposureBiasNum, *pulCount );
				break;

			case FJ_EXIF_TAG_MAXAPTURE_:  //MaxApertureValue
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.ExifIfd.MaxApertureValue.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.ExifIfd.ValueMaxApertureValue, *pulCount );
				break;

			case FJ_EXIF_TAG_METERMODE_:  // MeteringMode
				*pulCount = sizeof(SHORT);
				memcpy( g_bf_exif_param_buff, &img_detail.MeteringMode, *pulCount );
				break;

			case FJ_EXIF_TAG_LIGHTSOURCE_:  // LightSource
				*pulCount = sizeof(SHORT);
				memcpy( g_bf_exif_param_buff, &img_detail.LightSource, *pulCount );
				break;

			case FJ_EXIF_TAG_FLASH_:  // Flash
				*pulCount = sizeof(SHORT);
				memcpy( g_bf_exif_param_buff, &img_detail.flash, *pulCount );
				break;

			case FJ_EXIF_TAG_FOCALLEN_:  // FocalLength
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.ExifIfd.FocalLength.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.ExifIfd.FocalLength.ValueOffset.SValue.Value;
				break;

			case FJ_EXIF_TAG_MAKERNOTE_:  // MakerNote
				*pulCount = img_detail.maker_note_length;
				memcpy( g_bf_exif_param_buff, &img_detail.maker_note_offset, *pulCount );
				break;

			case FJ_EXIF_TAG_FLRESHPIXVER_:  // FlashPixVersion
//				*pulCount = gJpeg_Header.ExifIfd.FlashPixVersion.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.ExifIfd.FlashPixVersion.ValueOffset.LValue, *pulCount );
				break;

			case FJ_EXIF_TAG_COLORSPACE_:  // ColorSpace
//				*pulCount = sizeof(SHORT) * gJpeg_Header.ExifIfd.ColorSpace.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.ExifIfd.ColorSpace.ValueOffset.SValue.Value;
				break;

			case FJ_EXIF_TAG_PEXELXDIM_:  // PixelXDimension
				*pulCount = sizeof(SHORT);
				memcpy( g_bf_exif_param_buff, &img_detail.width, *pulCount );
				break;

			case FJ_EXIF_TAG_PEXELYDIM_:  // PixelYDimension
				*pulCount = sizeof(SHORT);
				memcpy( g_bf_exif_param_buff, &img_detail.lines, *pulCount );
				break;

			case FJ_EXIF_TAG_ITOPIFDPTR_:  // InteroperabilityIFDPointer
//				*pulCount = sizeof(LONG) * gJpeg_Header.ExifIfd.InteroperabilityIFDPointer.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.ExifIfd.InteroperabilityIFDPointer.ValueOffset.LValue;
				break;

			case 0xA215:  // ExposureIndex
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.ExifIfd.ExposureIndex.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.ExifIfd.ValueExposureBiasValue, *pulCount );
				break;

			case FJ_EXIF_TAG_SENSOR_:  // SensingMethod
//				*pulCount = sizeof(SHORT) * gJpeg_Header.ExifIfd.SensingMethod.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.ExifIfd.SensingMethod.ValueOffset.SValue.Value;
				break;

			case FJ_EXIF_TAG_FILESOURCE_:  // FileSource
//				*pulCount = gJpeg_Header.ExifIfd.FileSource.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.ExifIfd.FileSource.ValueOffset.LValue, *pulCount );
				break;

			case FJ_EXIF_TAG_SENCETYPE_:  // SceneType
//				*pulCount = gJpeg_Header.ExifIfd.SceneType.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.ExifIfd.SceneType.ValueOffset.LValue, *pulCount );
				break;

			case FJ_EXIF_TAG_EXPOSUREMODE_:  // ExposureMode
//				*pulCount = sizeof(SHORT) * gJpeg_Header.ExifIfd.ExposureMode.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.ExifIfd.ExposureMode.ValueOffset.SValue.Value;
				break;

			case FJ_EXIF_TAG_WB_:  // WhiteBalanceMode
				*pulCount = sizeof(SHORT);
				memcpy( g_bf_exif_param_buff, &img_detail.WBMode, *pulCount );
				break;

			case FJ_EXIF_TAG_DZOOM_:  // DigitalZoomRatio
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.ExifIfd.DigitalZoomRatio.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.ExifIfd.ValueDigitalZoomRatio, *pulCount );
				break;

			case FJ_EXIF_TAG_SHARPNESS_:  // Sharpness
//				*pulCount = sizeof(SHORT) * gJpeg_Header.ExifIfd.Sharpness.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.ExifIfd.Sharpness.ValueOffset.SValue.Value;
				break;

			default:
				return FJ_ERR_NG;
		}
	}
	else if( uwIfdIndex == FJ_IFD_INTEROPERABILITY ){
		switch( uwTagId ){
			case FJ_ITOP_TAG_ITOPINDEX_:  // InteroperabilityIndex
//				*pulCount = sizeof(DCF_ASCII) * gJpeg_Header.R98Ifd.InteroperabilityIndex.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.R98Ifd.InteroperabilityIndex.ValueOffset.SValue.Value, *pulCount );
				break;

			case FJ_ITOP_TAG_ITOPVERSION_:  // InteroperabilityVersion
//				*pulCount = gJpeg_Header.R98Ifd.InteroperabilityVersion.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.R98Ifd.InteroperabilityVersion.ValueOffset.LValue, *pulCount );
				break;

			default:
				return FJ_ERR_NG;
		}
	}
	else if( uwIfdIndex == FJ_IFD_1 ){
		switch( uwTagId ){
			case FJ_IFD1_TAG_COMPRESS_:  // Compression
//				*pulCount = sizeof(SHORT) * gJpeg_Header.Ifd1.Compression.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.Ifd1.Compression.ValueOffset.SValue.Value;
				break;

			case FJ_IFD1_TAG_ORIENT_:  // Orientation
				*pulCount = sizeof(SHORT);
				memcpy( g_bf_exif_param_buff, &img_detail.rotate_thumb, *pulCount );
				break;

			case FJ_IFD1_TAG_XRES_:  // XResolution
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.Ifd1.XResolution.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.Ifd1.ValueXResolution, *pulCount );
				break;

			case FJ_IFD1_TAG_YRES_:  // YResolution
//				*pulCount = sizeof(T_BF_DCF_EXIF_RATIONAL) * gJpeg_Header.Ifd1.YResolution.Count;
//				memcpy( g_bf_exif_param_buff, &gJpeg_Header.Ifd1.ValueYResolution, *pulCount );
				break;

			case FJ_IFD1_TAG_RESUINT_:  // ResolutionUnit
//				*pulCount = sizeof(SHORT) * gJpeg_Header.Ifd1.ResolutionUnit.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.Ifd1.ResolutionUnit.ValueOffset.SValue.Value;
				break;

			case FJ_IFD1_TAG_JPG_INTERCHGFMT_:  // JPEGInterchangeFormat
//				*pulCount = sizeof(LONG) * gJpeg_Header.Ifd1.JPEGInterchangeFormat.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.Ifd1.JPEGInterchangeFormat.ValueOffset.LValue;
				break;

			case FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_:  //JPEGInterchangeFormatLength
//				*pulCount = sizeof(LONG) * gJpeg_Header.Ifd1.JPEGInterchangeFormatLength.Count;
//				*g_bf_exif_param_buff = gJpeg_Header.Ifd1.JPEGInterchangeFormatLength.ValueOffset.LValue;
				break;

			default:
				return FJ_ERR_NG;
		}
	}
	else{
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;

}
#endif


/**
 * @brief		updated EXIF header
 * @param		INT32	IfdIndex
 *				INT32	TagId
 *				INT32	Type
 *				INT32	Count
 *				INT32	SupportLevel
 * @return		Success/Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_setimageexif( USHORT IfdIndex, USHORT TagId, USHORT Type, USHORT Count, USHORT SupportLevel, BYTE* Data )
{
	if(Type == 0xFFFF){
		switch( TagId ){
			/* essential Tag */
			/*			case FJ_IFD0_TAG_XRES_ :
						case FJ_IFD0_TAG_YRES_ :
						case FJ_IFD0_TAG_RESUNIT_ :
						case FJ_IFD0_TAG_YCBCRPOS_ :
						case FJ_IFD0_TAG_EXIFPTR_ :
						case FJ_EXIF_TAG_EXIFVER_ :
						case FJ_EXIF_TAG_COMPCONFIGURE_ :
						case FJ_EXIF_TAG_FLRESHPIXVER_ :
						case FJ_EXIF_TAG_COLORSPACE_ :
						case FJ_EXIF_TAG_PEXELXDIM_ :
						case FJ_EXIF_TAG_PEXELYDIM_ :
						case FJ_IFD1_TAG_COMPRESS_ :
						case FJ_IFD1_TAG_XRES_ :
						case FJ_IFD1_TAG_YRES_ :
						case FJ_IFD1_TAG_RESUINT_ :
						case FJ_IFD1_TAG_JPG_INTERCHGFMT_ :
						case FJ_IFD1_TAG_JPG_INTERCHGFMTLEN_ :
			*/
			case FJ_IFD0_TAG_MAKE_: // FALL THROUGH
			case FJ_IFD0_TAG_MODEL_: // FALL THROUGH
			case FJ_IFD0_TAG_ORIENT_: // FALL THROUGH
			case FJ_IFD0_TAG_IMGDESC_: // FALL THROUGH
			case FJ_IFD0_TAG_DATETIME_: // FALL THROUGH
			case FJ_IFD0_TAG_COPYRIGHT_: // FALL THROUGH
			case FJ_IFD0_TAG_SOFT_: // FALL THROUGH
			case FJ_IFD0_TAG_PRINTIM_: // 0xC4A5   FALL THROUGH
			case FJ_EXIF_TAG_EXPTIME_: // FALL THROUGH
			case FJ_EXIF_TAG_FNUM_: // FALL THROUGH
			case FJ_EXIF_TAG_EXPPROG_: // FALL THROUGH
			case FJ_EXIF_TAG_SHUTTER_: // FALL THROUGH
			case FJ_EXIF_TAG_APERTURE_: // FALL THROUGH
			case FJ_EXIF_TAG_ISOSPEEDRATE_: // FALL THROUGH
			case FJ_EXIF_TAG_DATETIMEORIG_: // FALL THROUGH
			case FJ_EXIF_TAG_DATETIMEDITI_: // FALL THROUGH
			case FJ_EXIF_TAG_COMPRESSBPP_: // FALL THROUGH
			case FJ_EXIF_TAG_EXPBIAS_: // FALL THROUGH
			case FJ_EXIF_TAG_MAXAPTURE_: // FALL THROUGH
			case FJ_EXIF_TAG_METERMODE_: // FALL THROUGH
			case FJ_EXIF_TAG_LIGHTSOURCE_: // FALL THROUGH
			case FJ_EXIF_TAG_FOCALLEN_: // FALL THROUGH
			case FJ_EXIF_TAG_ITOPIFDPTR_: // FALL THROUGH
			case FJ_EXIF_TAG_DZOOM_: // FALL THROUGH
			case FJ_EXIF_TAG_FOCALLENGTH35mm_: // FALL THROUGH
			case FJ_EXIF_TAG_MAKERNOTE_: // FALL THROUGH
			case FJ_EXIF_TAG_FLASH_: // FALL THROUGH
			case FJ_EXIF_TAG_SENSOR_: // FALL THROUGH
			case FJ_EXIF_TAG_FILESOURCE_: // FALL THROUGH
			case FJ_EXIF_TAG_SENCETYPE_: // FALL THROUGH
			case FJ_EXIF_TAG_CUSTOMRENDERED_: // 0xA401   FALL THROUGH
			case FJ_EXIF_TAG_EXPOSUREMODE_: // FALL THROUGH
			case FJ_EXIF_TAG_WB_: // FALL THROUGH
			case FJ_EXIF_TAG_CAPTURETYPE_: // FALL THROUGH
			case FJ_EXIF_TAG_CONTRAST_: // 0xA408   FALL THROUGH
			case FJ_EXIF_TAG_SATURATION_: // 0xA409   FALL THROUGH
			case FJ_EXIF_TAG_SHARPNESS_: // FALL THROUGH
			case FJ_ITOP_TAG_ITOPINDEX_: // FALL THROUGH
			case FJ_ITOP_TAG_ITOPVERSION_: // FALL THROUGH
			// case FJ_IFD1_TAG_MAKER1_: // FALL THROUGH  // same TagID as FJ_IFD0_TAG_MAKE_
			// case FJ_IFD1_TAG_MODEL1_: // FALL THROUGH  // same TagID as FJ_IFD0_TAG_MODEL_
			// case FJ_IFD1_TAG_ORIENT_: // FALL THROUGH  // same TagID as FJ_IFD0_TAG_ORIENT_
			// case FJ_IFD1_TAG_DATETIME1_: // FALL THROUGH  // same TagID as FJ_IFD0_TAG_DATETIME_
				break;

			default:
#ifdef FJ_DEBUG_PRINT_ON
//					BF_Debug_Print_Error(("must not delete this tag.  TagID = 0x%X\n", TagId ));
#endif
				return FJ_ERR_NG;
		}
	}

	BF_Dcf_Exif_Set_Value( IfdIndex, TagId, Type, Count, (FW_UCHAR*)Data );

	return FJ_ERR_OK;
}

/**
 * @brief		updated EXIF header for MPF class1
 * @param		INT32	IfdIndex
 *				INT32	TagId
 *				INT32	Type
 *				INT32	Count
 *				INT32	SupportLevel
 * @return		Success/Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_setimageexif_class1( USHORT IfdIndex, USHORT TagId, USHORT Type, USHORT Count, USHORT SupportLevel, BYTE* Data )
{
	if(Type == 0xFFFF){
		switch( TagId ){
			case FJ_IFD0_TAG_XRES_:
			case FJ_IFD0_TAG_YRES_:
			case FJ_IFD0_TAG_RESUNIT_:
			case FJ_EXIF_TAG_MAKERNOTE_:
			case FJ_EXIF_TAG_PEXELXDIM_:
			case FJ_EXIF_TAG_PEXELYDIM_:
			case FJ_EXIF_TAG_IMAGEUID_:
				break;
			default :
#ifdef FJ_DEBUG_PRINT_ON
//				BF_Debug_Print_Error(("must not delete this tag.  TagID = 0x%X\n", TagId ));
#endif
				return FJ_ERR_NG;
		}
	}

	BF_Dcf_Exif_Set_Value_Mpf(0, IfdIndex, TagId, Type, Count, (FW_UCHAR*)Data );

	return FJ_ERR_OK;
}

/**
 * @brief		updated EXIF header for MPF class2
 * @param		INT32	IfdIndex
 *				INT32	TagId
 *				INT32	Type
 *				INT32	Count
 *				INT32	SupportLevel
 * @return		Success/Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_setimageexif_class2( USHORT IfdIndex, USHORT TagId, USHORT Type, USHORT Count, USHORT SupportLevel, BYTE* Data )
{
	if(Type == 0xFFFF){
		switch( TagId ){
			case FJ_IFD0_TAG_XRES_:
			case FJ_IFD0_TAG_YRES_:
			case FJ_IFD0_TAG_RESUNIT_:
			case FJ_EXIF_TAG_MAKERNOTE_:
			case FJ_EXIF_TAG_PEXELXDIM_:
			case FJ_EXIF_TAG_PEXELYDIM_:
			case FJ_EXIF_TAG_IMAGEUID_:
				break;
			default :
#ifdef FJ_DEBUG_PRINT_ON
//				BF_Debug_Print_Error(("must not delete this tag.  TagID = 0x%X\n", TagId ));
#endif
				return FJ_ERR_NG;
		}
	}

	BF_Dcf_Exif_Set_Value_Mpf(1, IfdIndex, TagId, Type, Count, (FW_UCHAR*)Data );

	return FJ_ERR_OK;
}

#ifdef FJ_DEBUG_PRINT_ON
INT32	BF_Exif_Get_Exif_Info_Jpeg_Name( CHAR* file_path )
{
	T_BF_DCF_IF_IMAGE_DETAIL img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA img_data;

	BF_Dcf_If_Get_Exif_Info_Jpeg_Name( (FW_CHAR*)file_path, &img_detail, &img_data );

	return 0;
}
#endif // FJ_DEBUG_PRINT_ON

