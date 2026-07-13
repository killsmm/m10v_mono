/**
 * @file		playback.c
 * @brief		public playback function for host side
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_PB_INIT"
//#pragma arm section rodata="BASEFW_PB_CONST"
//#pragma arm section code="BASEFW_PB_CODE"

#include "playback.h"
#include "play_attr.h"
#include "play_index.h"
#include "play_jpeg.h"
#include "play_index.h"
#include "play_single.h"
#include "bf_startup.h"
#include "h264_user.h"
#include "fj_filesystem.h"
#include "fj_host.h"
#include "osd_attr.h"
#include "mba_if.h"

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration                                                          */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure                                                            */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data                                                          */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
/**
 * @brief		update hdmi display parameter
 * @param		None
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID bf_playback_update_disp_hdmi(VOID)
{
	INT32								ercd;
	T_IM_DISP_CTRL_MAIN_LAYER			ctrl;
	UINT32								hdmi_tv_layer = 0;
	T_IM_DISP_CTRL_OUTPUT_TRG_LIMIT		ctrl_out;
	T_IM_DISP_CTRL_OSD_LAYER			ctrl_osd[ E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_END ];
	
	if( BF_Play_Macro_Wrap_Get_Effective_Device() & E_BF_PLAY_MACRO_WRAP_EFFECTIVE_HDMI ) {
		//Set YCC 601->RGB
		Im_DISP_Change_CSC_Matrix( E_IM_DISP_HDMI, E_IM_DISP_SEL_LAYER_MAIN,E_IM_DISP_CC_MATRIX_NOCONVERT);

		ercd = Im_DISP_Get_Ctrl_Main_Layer( E_IM_DISP_HDMI,  NULL, &ctrl );
		if( ercd != D_DDIM_OK ) {
			BF_Debug_Print_Error(( "bf_playback_update_disp_hdmi(): Im_DISP_Get_Ctrl_Main_Layer err, err = 0x%x", ercd ));
			return;
		}

		ctrl.lrsz1.bit.HRSZM = 1;
		ctrl.lrsz1.bit.HRSZN = 1;
		ctrl.lrsz1.bit.HRSZOF = 0;
		ctrl.lrsz2.bit.VRSZM = 1;
		ctrl.lrsz2.bit.VRSZN = 1;

		// set disp parameter
		ercd = Im_DISP_Ctrl_Main_Layer( E_IM_DISP_HDMI, NULL, &ctrl );
		if( ercd != D_DDIM_OK ) {
			BF_Debug_Print_Error(( "bf_playback_update_disp_hdmi(): Im_DISP_Get_Ctrl_Main_Layer err, err = 0x%x", ercd ));
			return;
		}

		ercd = Im_DISP_Get_Ctrl_Output( E_IM_DISP_HDMI, &ctrl_out, NULL );
		if( ercd != D_DDIM_OK ) {
			BF_Debug_Print_Error(( "bf_playback_update_disp_hdmi(): Im_DISP_Get_Ctrl_Output err, err = 0x%x\n", ercd ));
			return;
		}

		ercd = Im_DISP_Get_Ctrl_OSD_Layer( E_IM_DISP_HDMI,  E_IM_DISP_SEL_LAYER_OSD_0, NULL, &ctrl_osd[ E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0 ] );
		if( ercd != D_DDIM_OK ) {
			BF_Debug_Print_Error(( "bf_playback_update_disp_hdmi(): Im_DISP_Get_Ctrl_OSD_Layer err, err = 0x%x\n", ercd ));
			return;
		}

		ercd = Im_DISP_Get_Ctrl_OSD_Layer( E_IM_DISP_HDMI,  E_IM_DISP_SEL_LAYER_OSD_1, NULL, &ctrl_osd[ E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1 ] );
		if( ercd != D_DDIM_OK ) {
			BF_Debug_Print_Error(( "bf_playback_update_disp_hdmi(): Im_DISP_Get_Ctrl_OSD_Layer err, err = 0x%x\n", ercd ));
			return;
		}

		if ((ctrl_out.tsl != D_IM_DISP_TSL_INTERLACE) && (ctrl_out.ovsize	== (LINES_FHD / 2))){
			Im_DISP_Stop( hdmi_tv_layer, E_IM_DISP_SEL_LAYER_NONE, 0, E_IM_DISP_STOP_TYPE_WITH_WAIT );

			ercd = Im_DISP_Wait_Stop( hdmi_tv_layer, E_IM_DISP_SEL_LAYER_NONE);
			if( ercd != D_DDIM_OK ) {
				BF_Debug_Print_Error(( "bf_playback_update_disp_hdmi(): Im_DISP_Wait_Stop, err = 0x%x\n", ercd ));
				return;
			}

			ctrl_out.tsl	= D_IM_DISP_TSL_INTERLACE;
			ctrl_out.ovsize	= ctrl_out.ovsize * 2;
			ctrl_osd[ E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0 ].grrsz2.bit.VRSZN	= 4;
			ctrl_osd[ E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1 ].grrsz2.bit.VRSZN	= 4;

			ercd = Im_DISP_Ctrl_Output( E_IM_DISP_HDMI, &ctrl_out, NULL );
			if( ercd != D_DDIM_OK ) {
				BF_Debug_Print_Error(( "bf_playback_update_disp_hdmi(): Im_DISP_Ctrl_Output err, err = 0x%x\n", ercd ));
				return;
			}

			ercd = Im_DISP_Ctrl_OSD_Layer( E_IM_DISP_HDMI, E_IM_DISP_SEL_LAYER_OSD_0, NULL, &ctrl_osd[ E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0 ] );
			if( ercd != D_DDIM_OK ) {
				BF_Debug_Print_Error(( "bf_playback_update_disp_hdmi(): Im_DISP_Ctrl_OSD_Layer err, err = 0x%x\n", ercd ));
				return;
			}

			ercd = Im_DISP_Ctrl_OSD_Layer( E_IM_DISP_HDMI, E_IM_DISP_SEL_LAYER_OSD_1, NULL, &ctrl_osd[ E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1 ] );
			if( ercd != D_DDIM_OK ) {
				BF_Debug_Print_Error(( "bf_playback_update_disp_hdmi(): Im_DISP_Ctrl_OSD_Layer err, err = 0x%x\n", ercd ));
				return;
			}

			Im_DISP_Start( hdmi_tv_layer, E_IM_DISP_SEL_LAYER_NONE, 0 );
		}
	}
}

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/
/******************************/
/**  PB Section(FastBoot)    **/
/******************************/
//#pragma arm section code="BASEFW_COMM_CODE"

/**
 * @brief		set current image index
 * @param		i:INT32 ImageIndex
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_setcurrentimage( INT32 ImageIndex )
{
	UCHAR  error = 0;
	USHORT dir_index;
	USHORT obj_index;

	dir_index = (ImageIndex & 0xFFFF0000) >> 16;
#if CO_DCF_EXIF_ON // for date DB
	if( (dir_index == 0) || (dir_index > 900) ){
#else
	if( dir_index > 900 ){
#endif // for date DB
		error = 1;
	}
	obj_index = ImageIndex & 0x0000FFFF;
#if CO_DCF_EXIF_ON // for date DB
	if( (obj_index == 0) || (obj_index > FJ_CUSTOM_DCFDB_FILENUM_MAX) ){
#else
	if( obj_index > FJ_CUSTOM_DCFDB_FILENUM_MAX ){
#endif // for date DB
		error = 1;
	}
	if( error != 0 ){
		BF_Debug_Print_Error(("image index error! (ImageIndex=0x%08X)\n", ImageIndex ));
		return FJ_ERR_NG;
	}

	BF_Play_Attr_Set_Image_Index( ImageIndex );

// --- REMOVE_DCF_EXIF BEGIN ---
	BF_Exif_Reset_Index();
// --- REMOVE_DCF_EXIF END ---
	
	return FJ_ERR_OK;
}

/**
 * @brief		get current image index
 * @param		None
 * @return		INT32 ImageIndex
 * @note		None
 * @attention	None
 */
INT32 fj_getcurrentimage( VOID )
{
	return BF_Play_Attr_Get_Image_Index();
}

/**
 * @brief set file type
 * @param VOID
 * @return None
 * @note None
 * @attention None
 */
FJ_ERR_CODE fj_setfiletype( USHORT FileType )
{
// --- REMOVE_DCF_EXIF BEGIN ---
	UINT32		err = 0;
	FW_UCHAR	ext_type;

	M_MBA_PRINTF(( D_MBA_KIND_CAPTURE, FJ_MBALOG_FUNC_ID_HOSTFUNC1, FJ_MBALOG_TYPE_START, "Dcf_If_Get_Type_By_Ext_Name" ));
	switch (FileType){
		case FJ_FILE_TYPE_JPEG:		// JPEG
			err = BF_Dcf_If_Get_Type_By_Ext_Name( "JPG", &ext_type );
			break;
			
//#if ( CO_MOVIE_FF_AUDIO_PATTERN == 0 )
		case FJ_FILE_TYPE_MP4:		// MP4
			err = BF_Dcf_If_Get_Type_By_Ext_Name( "MP4", &ext_type );
			break;
//#else
		case FJ_FILE_TYPE_MOV:		// MOV
			err = BF_Dcf_If_Get_Type_By_Ext_Name( "MOV", &ext_type );
			break;
//#endif
		case FJ_FILE_TYPE_RAW:		// RAW
			err = BF_Dcf_If_Get_Type_By_Ext_Name( "RAW", &ext_type );
			break;
			
		case FJ_FILE_TYPE_TIFF:		// TIFF
			err = BF_Dcf_If_Get_Type_By_Ext_Name( "TIF", &ext_type );
			break;
			
		case FJ_FILE_TYPE_WAV:		// WAV
			err = BF_Dcf_If_Get_Type_By_Ext_Name( "WAV", &ext_type );
			break;
			
		case FJ_FILE_TYPE_AVI:		// AVI
			err = BF_Dcf_If_Get_Type_By_Ext_Name( "AVI", &ext_type );
			break;
			
		case FJ_FILE_TYPE_MPEG:		// MPEG
			err = BF_Dcf_If_Get_Type_By_Ext_Name( "MPG", &ext_type );
			break;
			
		case FJ_FILE_TYPE_ASF:		// ASF
			err = BF_Dcf_If_Get_Type_By_Ext_Name( "ASF", &ext_type );
			break;
			
		case FJ_FILE_TYPE_WMV:		// WMV
			err = BF_Dcf_If_Get_Type_By_Ext_Name( "WMV", &ext_type );
			break;
			
		case FJ_FILE_TYPE_WMA:		// WMA
			err = BF_Dcf_If_Get_Type_By_Ext_Name( "WMA", &ext_type );
			break;
			
		case FJ_FILE_TYPE_TXT:		// TXT
			err = BF_Dcf_If_Get_Type_By_Ext_Name( "TXT", &ext_type );
			break;
			
		default:
			BF_Debug_Print_Error(("fj_setfiletype() case NG!!! (FileType=0x%x) \n", FileType));
			return FJ_ERR_NG;
	}
	if (err != FSDCF_ERRNO_OK){
		return FJ_ERR_NG;
	}

	M_MBA_PRINTF(( D_MBA_KIND_CAPTURE, FJ_MBALOG_FUNC_ID_HOSTFUNC1, FJ_MBALOG_TYPE_END, "Dcf_If_Get_Type_By_Ext_Name" ));

	BF_Play_Attr_Set_File_Type( ext_type );
	
// --- REMOVE_DCF_EXIF END ---
	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_PB_CODE"

/**
 * @brief		get file attributes
 * @param		i:FILE_ATTRIB* Attribute
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_getfileattributes( FILE_ATTRIB* Attribute )
{
// --- REMOVE_DCF_EXIF BEGIN ---
	T_BF_FS_IF_STAT fsif_stat;
#ifdef ATT_FILE_NAME_FULLPATH
	FW_CHAR file_path[ATT_FILE_NAME_SIZE];
#else
	FW_CHAR file_path[30];
	CHAR file_name = 0;
#endif

	if( BF_Play_Attr_Get_Image_Index() == 0 ){
		return FJ_ERR_NG;
	}

	BF_Dcf_If_Fstat(BF_Play_Attr_Get_Image_Index(), &fsif_stat);
#ifdef ATT_FILE_NAME_FULLPATH
	if( BF_Dcf_If_Get_Index_File_Path( BF_Play_Attr_Get_Image_Index(), NULL, file_path ) != 0 ){
		return FJ_ERR_NG;
	}
#endif	/* ATT_FILE_NAME_FULLPATH */

#ifdef ATT_FILE_NAME_FULLPATH
	strcpy( (char*)Attribute->name, file_path );
#else
	file_name = strlen( fsif_stat.fname );
	if( file_name < D_BF_PLAY_COMMON_FILE_NAME_LENGTH ){
		return FJ_ERR_NG;
	}

	strcpy( (char*)Attribute->name, fsif_stat.fname );
#endif
	Attribute->attributes = fsif_stat.attributes;
	Attribute->fileSize   = fsif_stat.fileSize;
	Attribute->updateTime = fsif_stat.time;
	Attribute->updateDate = fsif_stat.date;

// --- REMOVE_DCF_EXIF END ---
	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_COMM_CODE"

/**
 * @brief		get image attributes
 * @param		i:IMAGE_ATTRIBUTES* ImageAttr
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_getimageattributes( IMAGE_ATTRIBUTES* ImageAttr )
{
// --- REMOVE_DCF_EXIF BEGIN ---
	INT32 ret ;
	CHAR  flag;
	INT32 count;
	UCHAR model[] = {"FJ_MILB"};

	T_BF_DCF_IF_IMAGE_DETAIL		img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA		load_image_data;

	switch( BF_Dcf_If_Convert_File_Type( BF_Play_Attr_Get_File_Type() ) ){
		case E_BF_DCF_IF_FILE_TYPE_MP4:		// Fallthrough
		case E_BF_DCF_IF_FILE_TYPE_MOV:
#if 0 // PORTING
			ret = BF_H264_User_Play_Get_Image_Attr( ImageAttr );
			if( ret != 0 ){
				return FJ_ERR_NG;
			}
#endif
			break;
			
		case E_BF_DCF_IF_FILE_TYPE_WAV:
			break;
			
		default:
			/* get current exif info for jpeg */
			ret = BF_Dcf_If_Get_Exif_Info_Jpeg( BF_Play_Attr_Get_Image_Index(), &img_detail, &load_image_data );
			if( ret != 0){
				if ( ret != FSDCF_ERRNO_NOT_APP1 ){
					/* set play info */
					BF_Debug_Print_Error(("fj_getimageattributes(): BF_Dcf_If_Get_Exif_Info_Jpeg err\n"));
					return FJ_ERR_NG;
				}
			}

			/* set Image Attributes */
			if( img_detail.file_info.file_attr == E_BF_DCF_IF_ATTR_RDONLY ){
				ImageAttr->wFlags.pict_lock = 1;
			}
			else {
				ImageAttr->wFlags.pict_lock = 0;
			}
			// Audio Exists
			if( img_detail.file_info.option_file == E_BF_DCF_IF_OPTION_FILE_EXIST ){
				ImageAttr->wFlags.exists = 1;
			}
			else {
				ImageAttr->wFlags.exists = 0;
			}
			// Audio locked
			if( img_detail.file_info.file_attr == E_BF_DCF_IF_ATTR_RDONLY ){
				ImageAttr->wFlags.pudio_lock = 1;
			}
			else {
				ImageAttr->wFlags.pudio_lock = 0;
			}
			// captured with flash
			if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_FLASH_FLAG)) {
				if( (img_detail.flash & 0x01) == 1 ){
					ImageAttr->wFlags.flash = 1;
				}
				else {
					ImageAttr->wFlags.flash = 0;
				}
			}
			else {
				ImageAttr->wFlags.flash = 0;
			}
			// Thumbnail compressed
			if( load_image_data.thumb_offset != 0 ){
				ImageAttr->wFlags.thumb_comp = 1;
			}
			else {
				ImageAttr->wFlags.thumb_comp = 0;
			}
			// Captured by Milbeaut
			if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_MODEL_FLAG)) {
				if( (strlen( (char*)model )) == (strlen( (char*)img_detail.model ))){
					flag = 0x1;
					for(count = 0; count < strlen( (char*)model ); count++){
						if(model[count] != img_detail.model[count]){
							flag = 0x0;
							break;
						}
					}
					ImageAttr->wFlags.base_fw = flag;
				}
				else {
					ImageAttr->wFlags.base_fw = 0;
				}
			}
			else {
				ImageAttr->wFlags.base_fw = 0;
			}
			// Sub-sampling 4:2:2
			if( img_detail.ycc_format == 0x21 ){
				ImageAttr->wFlags.sampling = 1;
			}
			else {
				ImageAttr->wFlags.sampling = 0;
			}

			// Audio format supported
			ImageAttr->wFlags.audio_format = 0;

			// image compressed bits/pixel
			if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_COMPRESSBPP_FLAG)) {
				if( img_detail.bitperpixelDen != 0 ){
					ImageAttr->wFlags.bits_bixel = ( img_detail.bitperpixelNum / img_detail.bitperpixelDen );
				}
				else {
					ImageAttr->wFlags.bits_bixel = 0;
				}
			}
			else {
				ImageAttr->wFlags.bits_bixel = 0;
			}

#ifdef CO_BF_DCF_IF_APP11_ENABLE	// for Jpeg-HDR
			// set app11 exist flag
			ImageAttr->wFlags.app11_flg = img_detail.app11_flg;
#endif // CO_BF_DCF_IF_APP11_ENABLE
			
			if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_PEXELXDIM_FLAG)) {
				ImageAttr->wImageHSize = img_detail.width;
			}
			else{
				ImageAttr->wImageHSize = 0;
			}
			if(0 != (img_detail.exiftag_flg & FJ_EXIF_TAG_PEXELYDIM_FLAG)) {
				ImageAttr->wImageVSize = img_detail.lines;
			}
			else{
				ImageAttr->wImageVSize = 0;
			}
			ImageAttr->wThumbnailHSize = WIDTH_THUMB;
			ImageAttr->wThumbnailVSize = LINES_THUMB;
			if (img_detail.baseline_mp_flag != 0 ) {
				if (load_image_data.class1_offset != 0 ) {
					ImageAttr->wClass1HSize = img_detail.class1_width;
					ImageAttr->wClass1VSize = img_detail.class1_lines;
					if (load_image_data.class1_thumb_offset != 0 ) {
						ImageAttr->wClass1ThumHSize = WIDTH_THUMB;
						ImageAttr->wClass1ThumVSize = LINES_THUMB;
					}
					else {
						ImageAttr->wClass1ThumHSize = 0;
						ImageAttr->wClass1ThumVSize = 0;
					}
				}
				else {
					ImageAttr->wClass1HSize     = 0;
					ImageAttr->wClass1VSize     = 0;
					ImageAttr->wClass1ThumHSize = 0;
					ImageAttr->wClass1ThumVSize = 0;
				}
				if (load_image_data.class2_offset != 0 ) {
					ImageAttr->wClass2HSize = img_detail.class2_width;
					ImageAttr->wClass2VSize = img_detail.class2_lines;
					if (load_image_data.class2_thumb_offset != 0 ) {
						ImageAttr->wClass2ThumHSize = WIDTH_THUMB;
						ImageAttr->wClass2ThumVSize = LINES_THUMB;
					}
					else {
						ImageAttr->wClass2ThumHSize = 0;
						ImageAttr->wClass2ThumVSize = 0;
					}
				}
				else {
					ImageAttr->wClass2HSize     = 0;
					ImageAttr->wClass2VSize     = 0;
					ImageAttr->wClass2ThumHSize = 0;
					ImageAttr->wClass2ThumVSize = 0;
				}
			}
			else {
				ImageAttr->wClass1HSize     = 0;
				ImageAttr->wClass1VSize     = 0;
				ImageAttr->wClass1ThumHSize = 0;
				ImageAttr->wClass1ThumVSize = 0;
				ImageAttr->wClass2HSize     = 0;
				ImageAttr->wClass2VSize     = 0;
				ImageAttr->wClass2ThumHSize = 0;
				ImageAttr->wClass2ThumVSize = 0;
			}
			ImageAttr->ulClipTime      = 0;
			ImageAttr->ulTotalFrames   = 0;
			ImageAttr->ulTimeScale     = 0;
			ImageAttr->ulTimeIncrement = 0;
			break;
	}
// --- REMOVE_DCF_EXIF END ---
	return FJ_ERR_OK;
}

//#pragma arm section code="BASEFW_PB_CODE"


/**
 * @brief		set playback automatic display status
 * @param		i:E_PB_AUTODISP auto_disp
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_set_pb_autodisplay( E_PB_AUTODISP auto_disp )
{
	return BF_Set_Pb_AutoDisplay( auto_disp );
}

/**
 * @brief		get playback automatic display status
 * @param		o:E_PB_AUTODISP* auto_disp
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_get_pb_autodisplay( E_PB_AUTODISP* auto_disp )
{
	*auto_disp = BF_Get_Pb_AutoDisplay();
	if (*auto_disp >= E_PB_AUTODISP_ON) {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 * @brief		show playback displayed image
 * @param		i:E_PB_DISP_DEVICE disp_device
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_show_pb_dispimage( E_PB_DISP_DEVICE disp_device )
{
	E_FJ_DISP_IFS	show_enable;
	
	if (gBF_Play_Common_Display_Data.ercd != FJ_ERR_OK) {
		return FJ_ERR_NG;
	}
	
#if 0	// MILB Porting(To change to the appropriate value from later)
	show_enable = BF_Osd_Attr_Get_Display_Interface();	// LCD/HD connect status get
#else	// MILB Porting(To change to the appropriate value from later)
	show_enable = E_PB_DISP_DEVICE_LCD;
#endif	// MILB Porting(To change to the appropriate value from later)
	
	if (disp_device == E_PB_DISP_DEVICE_LCD) {
		if (show_enable == E_FJ_DISP_IFS_LCD) {		// LCD enable
			BF_Play_Macro_Wrap_Show_Lcd();
		}
		else {
			return FJ_ERR_NG;
		}
	}
	else if (disp_device == E_PB_DISP_DEVICE_HDMI) {	
		if (show_enable == E_FJ_DISP_IFS_HDMI) {	// HDMI enable
			BF_Play_Macro_Wrap_Show_Hdmi();
		}
		else {
			return FJ_ERR_NG;
		}
		
	}
	else {
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

/**
 * @brief		playback set display window parameters
 * @param		i:E_PB_DISP_WIN_TYPE window_type
 * @param		i:T_PB_DISP_WINDOW* disp_window
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_set_pb_display_window( E_PB_DISP_WIN_TYPE window_type, T_PB_DISP_WINDOW* disp_window )
{
	if (BF_Play_Common_Set_DisplayWindow( window_type, disp_window ) != FJ_ERR_OK) {
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

/**
 * @brief		playback set image window parameters
 * @param		i:E_PB_DISP_IMAGE disp_image
 * @param		i:T_PB_IMAGE_WINDOW* image_window
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_set_pb_image_window( E_PB_DISP_IMAGE disp_image, T_PB_IMAGE_WINDOW* image_window )
{
	if (BF_Play_Common_Set_ImageWindow( disp_image, image_window ) != FJ_ERR_OK) {
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

/**
 * @brief		playback cancel image window parameters
 * @param		None
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_can_pb_image_window( VOID )
{
	if (BF_Play_Common_Can_ImageWindow() != FJ_ERR_OK) {
		return FJ_ERR_NG;
	}
	
	return FJ_ERR_OK;
}

/**
 * @brief		playback get histogram size
 * @param		o:USHORT* width
 * @param		o:USHORT* lines
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_get_pb_histogram_size(USHORT* width, USHORT* lines)
{
	// set size
	*width = gBF_Play_Common_Display_Size.real_disp_width;
	*lines = gBF_Play_Common_Display_Size.real_disp_lines;

	return FJ_ERR_OK;
}

#if 0
/**
 * @brief		playback SingleDisplay
 * @param		None
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Playback_SingleDisplay(VOID)
{
	FJ_ERR_CODE ercd = FJ_ERR_NG;
	bf_playback_update_disp_hdmi();
	M_MBA_PRINTF(( D_MBA_KIND_CAPTURE, FJ_MBALOG_FUNC_ID_PB, FJ_MBALOG_TYPE_START, "Single" ));
	ercd = BF_Play_Single_Manage(0);
	M_MBA_PRINTF(( D_MBA_KIND_CAPTURE, FJ_MBALOG_FUNC_ID_PB, FJ_MBALOG_TYPE_END, "Single" ));

	return ercd;
}
#endif

/**
 * @brief		playback SingleDisplay
 * @param		None
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_playback_singledisplay(FJ_MODE screen_mode)
{
	FJ_ERR_CODE ercd = FJ_ERR_NG;
	
	bf_playback_update_disp_hdmi();
	BF_Play_Attr_Set_Screen_Mode(screen_mode);
	M_MBA_PRINTF(( D_MBA_KIND_CAPTURE, FJ_MBALOG_FUNC_ID_PB, FJ_MBALOG_TYPE_START, "Single" ));
	ercd = BF_Play_Single_Manage(0);
	M_MBA_PRINTF(( D_MBA_KIND_CAPTURE, FJ_MBALOG_FUNC_ID_PB, FJ_MBALOG_TYPE_END, "Single" ));

	return ercd;
}

/**
 * @brief		playback Main Image Load & Decode
 * @param		None
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Playback_Set_Main_Image(VOID)
{
	FJ_ERR_CODE ercd = FJ_ERR_NG;

	ercd = BF_Play_Jpeg_Set_Main_Image();

	return ercd;
}

#if 0
/**
 * @brief		playback ThumbnaiIndex
 * @param		None
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Playback_Thumbnailindex(VOID)
{
	FJ_ERR_CODE ercd = FJ_ERR_NG;

	ercd = BF_Play_Index_Manage();

	return ercd;
}
#endif

/**
 * @brief		playback ThumbnaiIndex
 * @param		None
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_playback_thumbnailindex(VOID)
{
	FJ_ERR_CODE ercd = FJ_ERR_NG;

	ercd = BF_Play_Index_Manage();

	return ercd;
}

//#pragma arm section code="BASEFW_COMM_CODE"
/**
 * @brief		set configration of thumbnail index display
 * @param		i:THUMBNAIL_CONFIGURATION* Configuration
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE fj_configthumbindex( THUMBNAIL_CONFIGURATION* Configuration )
{
	gBF_Play_Index_Thumbnail_Config.bThumbPerLine	= Configuration->bThumbPerLine;
	gBF_Play_Index_Thumbnail_Config.bThumbPerColumn	= Configuration->bThumbPerColumn;
	
	gBF_Play_Index_Thumbnail_Config.bFirstHGap		= Configuration->bFirstHGap;
	gBF_Play_Index_Thumbnail_Config.bHGap			= Configuration->bHGap;
	gBF_Play_Index_Thumbnail_Config.bLastHGap		= Configuration->bLastHGap;
	gBF_Play_Index_Thumbnail_Config.bFirstVGap		= Configuration->bFirstVGap;
	gBF_Play_Index_Thumbnail_Config.bVGap			= Configuration->bVGap;
	gBF_Play_Index_Thumbnail_Config.bLastVGap		= Configuration->bLastVGap;
	
	gBF_Play_Index_Thumbnail_Config.bFirstHGap_HD	= Configuration->bFirstHGap_HD;
	gBF_Play_Index_Thumbnail_Config.bHGap_HD		= Configuration->bHGap_HD;
	gBF_Play_Index_Thumbnail_Config.bLastHGap_HD	= Configuration->bLastHGap_HD;
	gBF_Play_Index_Thumbnail_Config.bFirstVGap_HD	= Configuration->bFirstVGap_HD;
	gBF_Play_Index_Thumbnail_Config.bVGap_HD		= Configuration->bVGap_HD;
	gBF_Play_Index_Thumbnail_Config.bLastVGap_HD	= Configuration->bLastVGap_HD;

	return FJ_ERR_OK;
}

