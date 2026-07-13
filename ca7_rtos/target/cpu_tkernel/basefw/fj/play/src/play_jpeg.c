/**
 * @file		play_jpeg.c
 * @brief		play jpeg process
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_PB_INIT"
//#pragma arm section rodata="BASEFW_PB_CONST"
//#pragma arm section code="BASEFW_PB_CODE"

#include "TKernel_id.h"

#include "sdram_map_cache_define.h"

#include "play_single.h"
#include "play_jpeg.h"
#include "play_attr.h"

#include "osd_histogram_comm.h"
#include "osd_attr.h"
#include "fj_host.h"

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
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
/******************************/
/**  PB Section(FastBoot)    **/
/******************************/
//#pragma arm section code="BASEFW_PB_CODE"
// --- REMOVE_DCF_EXIF BEGIN ---
/**
 * @brief		JPEG data JDSJP4 and JDSREC process to Play mode
 * @param		i:T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG*			jpg_dec_mng	JPEG decode DATA
 * @param		o:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* hd_param		JPEG display to HDMI/TV
 * @param		o:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* vga_param		JPEG display to LCD
 * @param		i:E_BF_DCF_IF_ROTATE rotate						thumbnail rotate
 * @return		0:normal end -1:abnormal end
 * @note		None
 * @attention	None
 */
static INT32 play_jpeg_convert_thumbnail( T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpg_dec_mng, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* hd_param, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* vga_param, E_BF_DCF_IF_ROTATE rotate )
{
	INT32							ret = 0;
	UCHAR							effective_device;
	E_BF_PLAY_MACRO_WRAP_PFMT		ycc_pfmt;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	src_param;
	T_BF_PLAY_MACRO_WRAP_SIZE		output_hd_size;

	BF_Play_Macro_Wrap_Set_Size_Addr( &src_param, jpg_dec_mng, jpg_dec_mng->width, jpg_dec_mng->lines, jpg_dec_mng->gl_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_JPEGMNG );

	BF_Play_Common_Change_YCC_Smpl_To_Pfmt( jpg_dec_mng->ycbcr_smpl, &ycc_pfmt );
	
	effective_device = BF_Play_Macro_Wrap_Get_Effective_Device();	// LCD/HD/TV connect status get

	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_HDMI & effective_device) {		// HDMI enable
		// Get output image size of HDMI
		if (BF_Play_Macro_Wrap_Get_Hdmi_Output_Size( &output_hd_size ) != FJ_ERR_OK) {
			BF_Debug_Print_Error(("play_jpeg_convert_thumbnail(): BF_Play_Macro_Wrap_Get_Hdmi_Output_Size error\n"));
			return FJ_ERR_NG;
		}

		hd_param->size.width = output_hd_size.width;
		hd_param->size.lines = output_hd_size.lines;
		
		ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( src_param, ycc_pfmt, hd_param, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, rotate, E_BF_PLAY_MACRO_WRAP_ADJUST_ON );
		if (ret != 0) {
			BF_Debug_Print_Error(("play_jpeg_convert_thumbnail(): BF_Play_Macro_Wrap_Rotate_Convert_Image error.\n"));
			return FJ_ERR_NG;
		}
	}
	
	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_LCD & effective_device) {		// LCD enable
		ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( src_param, ycc_pfmt, vga_param, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, rotate, E_BF_PLAY_MACRO_WRAP_ADJUST_ON );
		if (ret != 0) {
			BF_Debug_Print_Error(("play_jpeg_convert_thumbnail(): BF_Play_Macro_Wrap_Rotate_Convert_Image error.\n"));
			return FJ_ERR_NG;
		}
	}

	return FJ_ERR_OK;
}
// --- REMOVE_DCF_EXIF END ---

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/
/******************************/
/**  PB Section(FastBoot)    **/
/******************************/
//#pragma arm section code="BASEFW_PB_CODE"
/**
 * @brief		JPEG image display process for single
 * @param		None
 * @return		0:normal end -1:abnormal end
 * @note		None
 * @attention	None
 */
INT32 BF_Play_Jpeg_Single( VOID )
{
// --- REMOVE_DCF_EXIF BEGIN ---
	INT32							ret = 0;
	UCHAR							effective_device;
	ULONG							image_addr;
	T_BF_DCF_IF_IMAGE_DETAIL		img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA		load_image_data;
	E_BF_DCF_IF_ROTATE				current_rotate;
	E_BF_DCF_IF_ROTATE				thumb_rotate;
	FJ_MODE							screen_mode;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	vga_param;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	hd_param;

	memset( &img_detail,      0x00, sizeof(T_BF_DCF_IF_IMAGE_DETAIL) );
	memset( &load_image_data, 0x00, sizeof(T_BF_DCF_IF_LOAD_IMAGE_DATA) );
	
	gBF_Play_Common_Display_Data.panorama = 0;

	// Jpeg file size check
	if (BF_Play_Common_Check_Load_File_Size_Index( BF_Play_Attr_Get_Image_Index() ) != FJ_ERR_OK) {
		BF_Debug_Print_Error(("BF_Play_Jpeg_Single(): BF_Play_Common_Check_Load_File_Size_Index err\n"));
		return -1;
	}

	// get current exif information
	ret = BF_Dcf_If_Get_Exif_Info_Jpeg( BF_Play_Attr_Get_Image_Index(), &img_detail, &load_image_data );
	if (ret != 0) {
		if (ret != FSDCF_ERRNO_NOT_APP1) {	// exif format check
			BF_Debug_Print_Error(("BF_Play_Jpeg_Single(): BF_Dcf_If_Get_Exif_Info_Jpeg err\n"));
			return -1;
		}
	}

	// Check jpeg sub-sampling (YCC format)
	if (img_detail.rotate >= E_BF_DCF_IF_ROTATE_0) {	// Not JFIF format
		if (img_detail.jpeg_format == E_BF_DCF_IF_JPEG_FORMAT_EXIF) {	// Exif format exist
			if ((img_detail.ycc_format != 0x21) && (img_detail.ycc_format != 0x22)) {	// 0x21=YCC422, 0x22=YCC420
				BF_Debug_Print_Information(("BF_Play_Jpeg_Single(): Jpeg sub-sampling err\n"));
				return -1;
			}
		}
	}

	screen_mode = BF_Play_Attr_Get_Screen_Mode();

#ifdef CO_BF_DCF_IF_MPF_ENABLE
	/***********************************/
	/* Check Multi Picture Format file */
	/***********************************/
	if (img_detail.baseline_mp_flag == 0) {		// baseline MP file no exist
		switch (screen_mode) {
			case FJ_MODE_DISP_MONCLS1_MAIN:     // FALLTHROUGH
			case FJ_MODE_DISP_MONCLS1_THUMB:     // FALLTHROUGH
			case FJ_MODE_DISP_MONCLS2_MAIN:     // FALLTHROUGH
			case FJ_MODE_DISP_MONCLS2_THUMB:
				BF_Debug_Print_Error(("BF_Play_Jpeg_Single(): Jpeg MP format err\n"));
				return -1;

			default:
				break;
		}
	}
#endif

	/* current image size set */
	gBF_Play_Common_Image_Attr.img_hsize  = img_detail.width;
	gBF_Play_Common_Image_Attr.img_vsize  = img_detail.lines;
	gBF_Play_Common_Image_Attr.thumb_size = load_image_data.thumb_size;

	/* main rotate data set */
	BF_Play_Common_Rotate_Check_Exif_Tag( (E_BF_DCF_IF_ROTATE)img_detail.rotate, &current_rotate );
	gBF_Play_Common_Display_Data.current_rotate = (E_BF_DCF_IF_ROTATE)current_rotate;
	gBF_Play_Common_Display_Data.play_rotate    = (E_BF_DCF_IF_ROTATE)current_rotate;

	/* thumbnail rotate data set */
	BF_Play_Common_Rotate_Check_Exif_Tag( (E_BF_DCF_IF_ROTATE)img_detail.rotate_thumb, &thumb_rotate );
	gBF_Play_Common_Display_Data.current_rotate_thumb = (E_BF_DCF_IF_ROTATE)thumb_rotate;
	gBF_Play_Common_Display_Data.play_rotate_thumb    = (E_BF_DCF_IF_ROTATE)thumb_rotate;

	/* check jpeg image size */
	switch (screen_mode) {
		case FJ_MODE_DISP_IMAGE_MAIN :
		case FJ_MODE_RESTORE_MAIN_DECODE :
			if ((img_detail.width * img_detail.lines) > PIX_SIZ_PLAY_DEFAULT_SIZE) {
				BF_Debug_Print_Error(("BF_Play_Jpeg_Single(): Size Over Error\n"));
				return -9;
			}
			break;
			
#ifdef CO_BF_DCF_IF_MPF_ENABLE
		case FJ_MODE_DISP_MONCLS2_MAIN :
			if ((img_detail.class2_width * img_detail.class2_lines) > PIX_SIZ_PLAY_DEFAULT_SIZE) {
				BF_Debug_Print_Error(("BF_Play_Jpeg_Single(): Size Over Error\n"));
				return -9;
			}
			break;
			
		case FJ_MODE_DISP_MONCLS1_MAIN :
			if ((img_detail.class1_width * img_detail.class1_lines) > PIX_SIZ_PLAY_DEFAULT_SIZE) {
				BF_Debug_Print_Error(("BF_Play_Jpeg_Single(): Size Over Error\n"));
				return -9;
			}
			break;
#endif	// CO_BF_DCF_IF_MPF_ENABLE
			
		default:
			break;
	}

	effective_device = BF_Play_Macro_Wrap_Get_Effective_Device();		// LCD/HD/TV connect status get
	
	/* play single */
	switch (screen_mode) {
#ifdef CO_BF_DCF_IF_MPF_ENABLE
		case FJ_MODE_DISP_MONCLS1_MAIN:     // FALLTHROUGH
		case FJ_MODE_DISP_MONCLS2_MAIN:     // FALLTHROUGH
#endif
		case FJ_MODE_DISP_IMAGE_MAIN:
			image_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Hd_Out_Addr() );
			BF_Play_Macro_Wrap_Set_Size_Addr( &hd_param, &image_addr, WIDTH_FHD, LINES_FHD, WIDTH_FHD, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );
			
			image_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Vga_Out_Addr() );
			BF_Play_Macro_Wrap_Set_Size_Addr( &vga_param, &image_addr, WIDTH_VGA, LINES_VGA, WIDTH_VGA, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );

			ret = BF_Play_Jpeg_Main( &hd_param, &vga_param, current_rotate );  // main image load and decode
			if (ret == D_BF_PLAY_JPEG_LOAD_STOP) {
				return 0;
			}
			else if (ret == -1) {
				return -1;
			}
			
			if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_HDMI & effective_device) {	// HDMI enable
				BF_Play_Macro_Wrap_Display_Hdmi( hd_param );	// main image display
			}
			if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_LCD & effective_device) {	// LCD enable
				BF_Play_Macro_Wrap_Display_Lcd( vga_param );	// main image display
			}
			break;

#ifdef CO_BF_DCF_IF_MPF_ENABLE
		case FJ_MODE_DISP_MONCLS1_THUMB:     // FALLTHROUGH
		case FJ_MODE_DISP_MONCLS2_THUMB:     // FALLTHROUGH
#endif
		case FJ_MODE_DISP_IMAGE_THUMB:
			if (img_detail.jpeg_format == E_BF_DCF_IF_JPEG_FORMAT_EXIF) {		// thumbnail image exist check
				image_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Hd_Out_Addr() );
				BF_Play_Macro_Wrap_Set_Size_Addr( &hd_param, &image_addr, WIDTH_FHD, LINES_FHD, WIDTH_FHD, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );
				
				image_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Vga_Out_Addr() );
				BF_Play_Macro_Wrap_Set_Size_Addr( &vga_param, &image_addr, WIDTH_VGA, LINES_VGA, WIDTH_VGA, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );

				ret = BF_Play_Jpeg_Thumbnail( &hd_param, &vga_param, thumb_rotate );
				if (ret != 0) {		// thumbnail image load
					gBF_Play_Common_Image_Attr.thumb_size = 0;
					return -1;
				}

				if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_HDMI & effective_device) {	// HDMI enable
					BF_Play_Macro_Wrap_Display_Hdmi( hd_param );	// thumbnail image display
				}
				if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_LCD & effective_device) {	// LCD enable
					BF_Play_Macro_Wrap_Display_Lcd( vga_param );	// thumbnail image display
				}
			}
			else {	// thumbnail image not exist 
				return -1;
			}
			break;

		default:
			return -1;
	}

#ifdef CO_BF_OSD_GENERAL_DEBUG_HISTOGRAM
		BF_Osd_Histogram_Comm_Show_Histogram(); //Show histogram
#endif
// --- REMOVE_DCF_EXIF END ---

	return 0;
}

// --- REMOVE_DCF_EXIF BEGIN ---
/**
 * @brief		JPEG-Original image display process for main
 * @param		o:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* hd_param
 * @param		o:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* vga_param
 * @param		i:E_BF_DCF_IF_ROTATE rotate
 * @return		0:normal end -1:abnormal end
 * @note		None
 * @attention	None
 */
INT32 BF_Play_Jpeg_Main( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* hd_param, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* vga_param, E_BF_DCF_IF_ROTATE rotate )
{
	INT32					ret;
	LONG					cb_ret; 
	UCHAR					effective_device;
	ULONG					make_addr;
	ULONG					half_rec_addr = 0;
	ULONG					load_addr;
	ULONG					decode_addr;
	FJ_MODE					screen_mode;
	T_BF_PLAY_MACRO_WRAP_SIZE			output_size;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR		hist;
	T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG	jpgmng;
	T_BF_DCF_IF_IMAGE_DETAIL			img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA			load_image_data;

	/* Interface of the following function(Trimming,Rotate,Resize,Startup Image,Effect) */
	memset( &jpgmng,          0x00, sizeof(T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG) );
	memset( &img_detail,      0x00, sizeof(T_BF_DCF_IF_IMAGE_DETAIL) );
	memset( &load_image_data, 0x00, sizeof(T_BF_DCF_IF_LOAD_IMAGE_DATA) );
	
	memset( &gBF_Play_Common_Edit_Image_Load, 0, sizeof(T_BF_PLAY_COMMON_EDITIMAGE_LOAD) );
	gBF_Play_Common_Edit_Image_Load.load_status = E_BF_PLAY_COMMON_LOAD_STATUS_NG;

	/***************************/
	/* JPEG Image Load Process */
	/***************************/
	/* get load area address */
	load_addr   = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Image_Load_Addr() );
	decode_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Image_Work_Addr() );

	screen_mode = BF_Play_Attr_Get_Screen_Mode();
	
	switch (screen_mode) {
		case FJ_MODE_DISP_IMAGE_MAIN:		// FALLTHROUGH
		case FJ_MODE_PLAYBACK:		// FALLTHROUGH
#ifdef CO_BF_DCF_IF_MPF_ENABLE
		case FJ_MODE_DISP_MONCLS1_MAIN:		// FALLTHROUGH
		case FJ_MODE_DISP_MONCLS2_MAIN:
#endif
			ret = BF_Play_Macro_Wrap_Get_Jpeg( screen_mode, BF_Play_Attr_Get_Image_Index(), (UCHAR*)load_addr, 
												(UCHAR*)decode_addr, &jpgmng, &img_detail, &load_image_data, TRUE, TRUE );
			if (ret != D_OS_USER_E_OK) {
				BF_Debug_Print_Error(("BF_Play_Jpeg_Main(): BF_Play_Macro_Wrap_Get_Jpeg err.\n"));
				return ret;
			}
			break;

		default:
			BF_Debug_Print_Error(("BF_Play_Jpeg_Main(): system mode err\n"));
			return -1;
	}

	// Exif CallBack
	if (gBF_Play_Common_Display_Data.play_file_info == E_BF_PLAY_COMMON_FILE_INFO_SINGLE &&
		(screen_mode == FJ_MODE_RESTORE_MAIN_DECODE || screen_mode == FJ_MODE_DISP_IMAGE_MAIN)) {
		make_addr = load_addr + img_detail.maker_note_offset;
		cb_ret = FJ_Host_ParseMakerNoteCB( (CHAR*)make_addr, (INT32)img_detail.maker_note_length, img_detail.maker_note_offset, img_detail.byte_order, NULL, 0 );
		if (cb_ret == 0xFFFFFFFF) {
			;// false chk
		}
	}

	if (jpgmng.width != gBF_Play_Common_Image_Attr.img_hsize) {
		gBF_Play_Common_Image_Attr.img_hsize = jpgmng.width;
	}
	if (jpgmng.lines != gBF_Play_Common_Image_Attr.img_vsize) {
		gBF_Play_Common_Image_Attr.img_vsize = jpgmng.lines;
	}

	// Convert file size to HD/LCD size
	half_rec_addr = BF_Sdram_Map_Play_Get_Half_Rec_Addr();		
	if (BF_Play_Common_Convert_2_Display( &jpgmng, hd_param, vga_param, half_rec_addr, rotate ) != FJ_ERR_OK) {
		return -1;
	}

	/* set global data */
	effective_device = BF_Play_Macro_Wrap_Get_Effective_Device();	// LCD/HD/TV connect status get
	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_HDMI & effective_device) {		// HDMI enable
		memcpy(&(gBF_Play_Common_Edit_Image_Load.hd_size_addr), hd_param, sizeof(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR));
	}
	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_LCD & effective_device) {		// LCD display
		memcpy(&(gBF_Play_Common_Edit_Image_Load.vga_size_addr), vga_param, sizeof(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR));
	}

	switch (screen_mode) {
		case FJ_MODE_DISP_IMAGE_MAIN:		// FALLTHROUGH
		case FJ_MODE_PLAYBACK:
			/* Interface of the following function (Trimming, Rotate, Resize, Startup Image, Effect) */
			memcpy( &(gBF_Play_Common_Edit_Image_Load.jpgmng), &jpgmng, sizeof(T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG) );
			gBF_Play_Common_Edit_Image_Load.load_status = E_BF_PLAY_COMMON_LOAD_STATUS_OK;
			break;

		default:
			gBF_Play_Common_Edit_Image_Load.load_status = E_BF_PLAY_COMMON_LOAD_STATUS_NG;
			break;
	}

	if(gBF_Play_Common_Display_Data.play_file_info == E_BF_PLAY_COMMON_FILE_INFO_SINGLE) {
		if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_HDMI & effective_device) {
			// Get output image size of HDMI
			if (BF_Play_Macro_Wrap_Get_Hdmi_Output_Size( &output_size ) != FJ_ERR_OK) {
				// Error
				BF_Debug_Print_Error(("BF_Play_Jpeg_Main(): BF_Play_Macro_Wrap_Get_Hdmi_Output_Size:HDMI status error\n"));
				return -1;
			}
			memcpy(&hist, hd_param, sizeof(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR));
		}
		else {
			output_size.width = D_WIDTH_VGA;
			output_size.lines = D_LINES_VGA;		
			memcpy(&hist, vga_param, sizeof(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR));
		}

		BF_Play_Macro_Wrap_Get_Histogram_Data( hist.size.width, hist.size.lines, hist.size.global_width, hist.addrs.addr_a );

		gBF_Play_Common_Display_Size.disp_width			= output_size.width;		/* pgr0872 */
		gBF_Play_Common_Display_Size.disp_lines			= output_size.lines;		/* pgr0872 */
		gBF_Play_Common_Display_Size.real_disp_width	= hist.size.width;
		gBF_Play_Common_Display_Size.real_disp_lines	= hist.size.lines;

		Im_Xch_Wait_End( E_IM_XCH_CH_SEL_0 );

		//Set Histogram data to OSD histogram's buffer
		BF_Osd_Histogram_Comm_Set_Histogram(NULL);

		Im_Xch_Close( E_IM_XCH_CH_SEL_0 );
	}

	return 0;
}

/**
 * @brief		JPEG-Original image display process for thumbnail
 * @param		o:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* hd_param
 * @param		o:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* vga_param
 * @param		i:E_BF_DCF_IF_ROTATE rotate
 * @return		0:normal end -1:abnormal end
 * @note		None
 * @attention	None
 */
INT32 BF_Play_Jpeg_Thumbnail( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* hd_param, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* vga_param, E_BF_DCF_IF_ROTATE rotate )
{
	INT32					ret;
	LONG					cb_ret;
	ULONG					load_addr;
	ULONG					decode_addr;
	ULONG					make_addr;
	FJ_MODE					screen_mode;
	T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG	jpgmng;
	T_BF_DCF_IF_IMAGE_DETAIL			img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA			load_image_data;

	memset( &jpgmng,          0x00, sizeof(T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG) );
	memset( &img_detail,      0x00, sizeof(T_BF_DCF_IF_IMAGE_DETAIL) );
	memset( &load_image_data, 0x00, sizeof(T_BF_DCF_IF_LOAD_IMAGE_DATA) );

	/*******************************/
	/* JPEG Thumbnail Load Process */
	/*******************************/
	/* get load area address */
	load_addr   = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Image_Load_Addr() );
	decode_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Image_Work_Addr() );

	screen_mode = BF_Play_Attr_Get_Screen_Mode();

	switch (screen_mode) {
		case FJ_MODE_DISP_IMAGE_THUMB:       // thumbnail load
#ifdef CO_BF_DCF_IF_MPF_ENABLE
		case FJ_MODE_DISP_MONCLS1_THUMB:
		case FJ_MODE_DISP_MONCLS2_THUMB:
#endif
			ret = BF_Play_Macro_Wrap_Get_Jpeg( screen_mode, BF_Play_Attr_Get_Image_Index(), (UCHAR*)load_addr, 
												(UCHAR*)decode_addr, &jpgmng, &img_detail, &load_image_data, TRUE, TRUE );
			if (ret != D_OS_USER_E_OK) {
				BF_Debug_Print_Error(("BF_Play_Jpeg_Thumbnail(): BF_Play_Macro_Wrap_Get_Jpeg err.\n"));
				return ret;
			}
			break;
			
		default:
			BF_Debug_Print_Error(("BF_Play_Jpeg_Thumbnail(): system mode err\n"));
			return -1;
	}

	// Exif CallBack
	if (gBF_Play_Common_Display_Data.play_file_info == E_BF_PLAY_COMMON_FILE_INFO_SINGLE) {
		make_addr = load_addr + img_detail.maker_note_offset;
		cb_ret = FJ_Host_ParseMakerNoteCB( (CHAR*)make_addr, (INT32)img_detail.maker_note_length, img_detail.maker_note_offset, img_detail.byte_order, NULL, 0 );
		if(cb_ret == 0xFFFFFFFF) {
			;// false chk
		}
	}

	/* JPEG Thumbnail Convert Process */
	ret = play_jpeg_convert_thumbnail( &jpgmng, hd_param, vga_param, rotate );
	if (ret != 0){
		return -1;
	}

	return 0;
}
// --- REMOVE_DCF_EXIF END ---

/**
 * @brief		JPEG-Original image set process for main
 * @param		None
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Jpeg_Set_Main_Image(VOID)
{
// --- REMOVE_DCF_EXIF BEGIN ---
	INT32								ret = 0;
	USHORT								current_filetype;
	ULONG								image_addr;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR		vga_param;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR		hd_param;
#ifdef D_PLAY_JPEG_MAIN_DECODE
	T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG	jpgmng;
#endif

	current_filetype = BF_Dcf_If_Convert_File_Type( BF_Play_Attr_Get_File_Type() );

	if(current_filetype != E_BF_DCF_IF_FILE_TYPE_JPG) {
		return FJ_ERR_NG;
	}

	gBF_Play_Common_Display_Data.play_file_info = E_BF_PLAY_COMMON_FILE_INFO_MAIN;
		if( (BF_Play_Attr_Get_Prev_Screen_Mode()) != FJ_MODE_DISP_IMAGE_MAIN) {
			image_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)SDRAM_ADR_PLAY_IMAGE_SAVE_AREA_HD );
			BF_Play_Macro_Wrap_Set_Size_Addr( &hd_param, &image_addr, WIDTH_FHD, LINES_FHD, WIDTH_FHD, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );
			
			image_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)SDRAM_ADR_PLAY_IMAGE_SAVE_AREA_VGA );
			BF_Play_Macro_Wrap_Set_Size_Addr( &vga_param, &image_addr, WIDTH_VGA, LINES_VGA, WIDTH_VGA, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );

			ret = BF_Play_Jpeg_Main( &hd_param, &vga_param, gBF_Play_Common_Display_Data.current_rotate );  // main image load and decode
			if (ret == D_BF_PLAY_JPEG_LOAD_STOP) {
				return FJ_ERR_OK;
			}
			else if (ret == -1) {
				return FJ_ERR_NG;
			} 
		}
	gBF_Play_Common_Display_Data.play_rotate		= gBF_Play_Common_Display_Data.current_rotate;
	gBF_Play_Common_Display_Data.play_rotate_thumb	= gBF_Play_Common_Display_Data.current_rotate_thumb;
	gBF_Play_Common_Display_Data.ercd				= FJ_ERR_OK;
	gBF_Play_Common_Display_Data.rotate_index		= 0;
	gBF_Play_Common_Display_Data.play_rotate_flg	= 0;

// --- REMOVE_DCF_EXIF END ---
	return FJ_ERR_OK;
}

