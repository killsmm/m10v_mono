/**
 * @file		play_single.c
 * @brief		playback single function
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_PB_INIT"
//#pragma arm section rodata="BASEFW_PB_CONST"
//#pragma arm section code="BASEFW_PB_CODE"

#include "play_attr.h"
#include "play_single.h"
#include "play_jpeg.h"
#include "fj_filesystem.h"
#include "movie_video_attr.h"
#include "h264_user.h"
#include "osd_attr.h"
#include "mode.h"
#include "sdram_map_cache_define.h"

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
typedef enum {
	E_BF_PLAY_SINGLE_LOCAL_RET_CODE_NORMAL_END		= 0,
	E_BF_PLAY_SINGLE_LOCAL_RET_CODE_FILE_LOAD_ERROR,    // -1 (add other error)
	E_BF_PLAY_SINGLE_LOCAL_RET_CODE_SIZE_ERROR,         // -9
	E_BF_PLAY_SINGLE_LOCAL_RET_CODE_NOT_SUPPORT_ERROR
} E_BF_PLAY_SINGLE_LOCAL_RET_CODE;

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
// --- REMOVE_DCF_EXIF BEGIN ---
/**
 * @brief		JPEG image display process for single
 * @param		i:UCHAR flg
 * @return		E_BF_PLAY_SINGLE_LOCAL_RET_CODE
 * @note		None
 * @attention	None
 */
static E_BF_PLAY_SINGLE_LOCAL_RET_CODE play_single_jpeg( UCHAR flg )
{
	INT32 ret;

	ret = BF_Play_Jpeg_Single();
	if (ret != 0){
#ifdef FJ_DEBUG_PRINT_ON
		BF_Debug_Print_Error(("play_single_jpeg(): Playback play_single_jpeg ERROR\n"));
#endif  /* FJ_DEBUG_PRINT_ON */
		if (ret != -9) {
			return E_BF_PLAY_SINGLE_LOCAL_RET_CODE_FILE_LOAD_ERROR;
		}
		else {
			return E_BF_PLAY_SINGLE_LOCAL_RET_CODE_SIZE_ERROR;
		}
	}
	return E_BF_PLAY_SINGLE_LOCAL_RET_CODE_NORMAL_END;
}

/**
 * @brief		Movie top image display process for single
 * @param		None
 * @return		E_BF_PLAY_SINGLE_LOCAL_RET_CODE
 * @note		None
 * @attention	None
 */
static E_BF_PLAY_SINGLE_LOCAL_RET_CODE play_single_movie(VOID)
{
#if 0 // PORTING
	INT32							ret;
	UCHAR							effective_device;
	E_BF_DCF_IF_FILE_TYPE			file_type;
	T_BF_H264_USER_TOP_IMAGE		h264_top_image;
	T_BF_PLAY_MACRO_WRAP_ADDR		src_addr;
	T_BF_PLAY_MACRO_WRAP_ADDR		dst_addr;
	E_BF_PLAY_MACRO_WRAP_PFMT		ycc_type;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	src;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	dst;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	mid;
	ULONG							image_hd_addr;
	ULONG							image_lcd_addr;
	ULONG							image_mid_addr;
	T_PB_IMAGE_SIZE					src_image;
	T_PB_IMAGE_SIZE					disp_image;
	
	memset( &src_image,  0, sizeof(T_PB_IMAGE_SIZE) );
	memset( &disp_image, 0, sizeof(T_PB_IMAGE_SIZE) );

	file_type = BF_Dcf_If_Convert_File_Type( BF_Play_Attr_Get_File_Type() );
	switch (file_type) {
		case E_BF_DCF_IF_FILE_TYPE_MP4 :		// Fallthrough
		case E_BF_DCF_IF_FILE_TYPE_MOV :
			h264_top_image.addr_y = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)SDRAM_ADR_PLAY_SINGLE_MOVIE_AREA_0 );
			h264_top_image.addr_c = h264_top_image.addr_y + PIX_SIZ_FHD;
			
			ret = BF_H264_User_Play_Top( D_BF_H264_USER_ON, &h264_top_image );
			if (ret != 0) {
#ifdef FJ_DEBUG_PRINT_ON
				BF_Debug_Print_Error(("play_single_movie(): Playback play_single_movie ERROR\n"));
#endif  /* FJ_DEBUG_PRINT_ON */
				if (ret != -9) {
					return E_BF_PLAY_SINGLE_LOCAL_RET_CODE_FILE_LOAD_ERROR;
				}
				else {
					return E_BF_PLAY_SINGLE_LOCAL_RET_CODE_SIZE_ERROR;
				}
			}

			if (h264_top_image.format == E_BF_H264_USER_IMAGE_FORMAT_420) {
				ycc_type	= E_BF_PLAY_MACRO_WRAP_PFMT_YCC420;
			}
			else if (h264_top_image.format == E_BF_H264_USER_IMAGE_FORMAT_422) {
				ycc_type	= E_BF_PLAY_MACRO_WRAP_PFMT_YCC422;
			}
			else {
				return E_BF_PLAY_SINGLE_LOCAL_RET_CODE_NOT_SUPPORT_ERROR;
			}

			if ((h264_top_image.scan_mode == E_BF_H264_USER_SCAN_MODE_INTERLACE) && (h264_top_image.width == WIDTH_FHD) && (h264_top_image.lines == LINES_FHD)){
				src_addr.addr_a	= h264_top_image.addr_y;
				src_addr.addr_b	= h264_top_image.addr_c;
				BF_Play_Macro_Wrap_Set_Size_Addr( &src, &src_addr, h264_top_image.width, (h264_top_image.lines)/2, BF_Play_Common_RoundUp_Bit_32(h264_top_image.width) * 2, E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR );
				dst_addr.addr_a = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)SDRAM_ADR_PLAY_SINGLE_MOVIE_AREA_2 );
				dst_addr.addr_b = dst_addr.addr_a + PIX_SIZ_FHD;
				BF_Play_Macro_Wrap_Set_Size_Addr( &dst, &dst_addr, h264_top_image.width, h264_top_image.lines, BF_Play_Common_RoundUp_Bit_32(h264_top_image.width), E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR );
				ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( src, ycc_type, &dst, ycc_type, E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF );
				if (ret != 0) {
					return E_BF_PLAY_SINGLE_LOCAL_RET_CODE_SIZE_ERROR;
				}				
				src_addr.addr_a		= dst.addrs.addr_a;
				src_addr.addr_b		= dst.addrs.addr_b;
			}
			else{
				src_addr.addr_a		= h264_top_image.addr_y;
				src_addr.addr_b		= h264_top_image.addr_c;
			}
							
			
			src_image.gl_width	= BF_Play_Common_RoundUp_Bit_32(h264_top_image.width);
			src_image.width		= h264_top_image.width;
			src_image.lines		= h264_top_image.lines;
			
			disp_image.gl_width	= BF_Play_Common_RoundUp_Bit_32(h264_top_image.display_width);
			disp_image.width	= h264_top_image.display_width;
			disp_image.lines	= h264_top_image.display_lines;
			
			BF_Play_Macro_Wrap_Set_Size_Addr( &src, &src_addr, src_image.width, src_image.lines, src_image.gl_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR );
			
			break;
			
		default :
			return E_BF_PLAY_SINGLE_LOCAL_RET_CODE_NOT_SUPPORT_ERROR;
	}	

	if ((src_image.width != disp_image.width) || (src_image.lines != disp_image.lines)) {
		image_mid_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)SDRAM_ADR_PLAY_SINGLE_MOVIE_AREA_1 );

		BF_Play_Macro_Wrap_Set_Size_Addr( &mid, &image_mid_addr, disp_image.width, disp_image.lines, disp_image.gl_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );
		ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( src, ycc_type, &mid, ycc_type, E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF );
		if (ret != 0) {
			return E_BF_PLAY_SINGLE_LOCAL_RET_CODE_SIZE_ERROR;
		}
	}
	
	effective_device = BF_Play_Macro_Wrap_Get_Effective_Device();	// LCD/HD/TV connect status get
	
	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_HDMI & effective_device) {		// HDMI enable
		T_BF_PLAY_MACRO_WRAP_SIZE	output_hd_size;
		
		BF_Play_Macro_Wrap_Get_Hdmi_Output_Size(&output_hd_size);
		
		image_hd_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Hd_Out_Addr() );
		
		dst_addr.addr_a = image_hd_addr;
		dst_addr.addr_b = dst_addr.addr_a + PIX_SIZ_FHD;
		BF_Play_Macro_Wrap_Set_Size_Addr( &dst, &dst_addr, output_hd_size.width, output_hd_size.lines, output_hd_size.global_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR );
		
		if ((src_image.width != disp_image.width) || (src_image.lines != disp_image.lines)) {
			ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( mid, ycc_type, &dst, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF );		/* pgr0000 */
		}
		else {
			ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( src, ycc_type, &dst, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_ON );
		}
		if (ret != 0) {
			return E_BF_PLAY_SINGLE_LOCAL_RET_CODE_SIZE_ERROR;
		}
		
		BF_Play_Macro_Wrap_Display_Hdmi( dst );		// top image display to HDMI
	}
	
	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_LCD & effective_device) {		// LCD enable
		image_lcd_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Vga_Out_Addr() );
		
		dst_addr.addr_a = image_lcd_addr;
		dst_addr.addr_b = dst_addr.addr_a +  PIX_SIZ_VGA;		
		BF_Play_Macro_Wrap_Set_Size_Addr( &dst, &dst_addr, WIDTH_VGA, LINES_VGA, WIDTH_VGA, E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR );

		if ((src_image.width != disp_image.width) || (src_image.lines != disp_image.lines)) {
			ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( mid, ycc_type, &dst, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF );		/* pgr0000 */
		}
		else {
			ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( src, ycc_type, &dst, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_ON );
		}
		if (ret != 0) {
			return E_BF_PLAY_SINGLE_LOCAL_RET_CODE_SIZE_ERROR;
		}
		
		BF_Play_Macro_Wrap_Display_Lcd( dst );	// top image display to LCD
	}
	
	
#endif // PORTING
	return E_BF_PLAY_SINGLE_LOCAL_RET_CODE_NORMAL_END;
}
// --- REMOVE_DCF_EXIF END ---

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/
/**
 * @brief		Single display process to Play mode
 * @param		i:UCHAR flg
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Single_Manage( UCHAR flg )
{
	FJ_ERR_CODE			ercd = FJ_ERR_OK;
// --- REMOVE_DCF_EXIF BEGIN ---
	E_BF_PLAY_SINGLE_LOCAL_RET_CODE		local_ret = E_BF_PLAY_SINGLE_LOCAL_RET_CODE_NORMAL_END;
	USHORT				current_filetype;

	if (BF_Fs_If_Check_Media_State( BF_Fs_If_Get_Cur_Media() ) != E_BF_MEDIA_STATE_MEDIA_STATE_INSERT) {
		return FJ_ERR_NG;
	}

	current_filetype = BF_Dcf_If_Convert_File_Type( BF_Play_Attr_Get_File_Type() );
	gBF_Play_Common_Display_Data.play_file_info = E_BF_PLAY_COMMON_FILE_INFO_SINGLE;
	/**************************/
	/* Load file data Process */
	/**************************/
	/* check current file type */
	switch (current_filetype) {
		case E_BF_DCF_IF_FILE_TYPE_JPG:
			local_ret = play_single_jpeg( flg );
			break;
			
		case E_BF_DCF_IF_FILE_TYPE_WAV:
			/* Set mode [FJ_MODE_MENU] */
#ifdef CO_AUTO_MODE_CHANGE_ON
			fj_setmode( FJ_MODE_MENU );
#else
			local_ret = E_BF_PLAY_SINGLE_LOCAL_RET_CODE_NOT_SUPPORT_ERROR;
#endif
			break;
			
		case E_BF_DCF_IF_FILE_TYPE_MP4:	// fallthrough
		case E_BF_DCF_IF_FILE_TYPE_MOV:
			if (fj_get_act_sysmode() == FJ_MODE_PLAYBACK) {
				local_ret = play_single_movie();
			}
			else {
#ifdef CO_AUTO_MODE_CHANGE_ON
				/* Set mode [FJ_MODE_PLAYBACK] */
				fj_setmode( FJ_MODE_PLAYBACK );
#else
				local_ret = E_BF_PLAY_SINGLE_LOCAL_RET_CODE_NOT_SUPPORT_ERROR;
#endif
			}
			break;
			
		default:
			local_ret = E_BF_PLAY_SINGLE_LOCAL_RET_CODE_FILE_LOAD_ERROR;
			break;
	}

	if (local_ret != E_BF_PLAY_SINGLE_LOCAL_RET_CODE_NORMAL_END) {
		ercd = FJ_ERR_NG;
		if (local_ret == E_BF_PLAY_SINGLE_LOCAL_RET_CODE_SIZE_ERROR) {
#ifdef FJ_DEBUG_PRINT_ON
			BF_Debug_Print_Error(("BF_Play_Single_Manage(): Playback SIZE_ERROR\n"));
#endif  /* FJ_DEBUG_PRINT_ON */
		}
		else if (local_ret == E_BF_PLAY_SINGLE_LOCAL_RET_CODE_NOT_SUPPORT_ERROR) {
#ifdef FJ_DEBUG_PRINT_ON
			BF_Debug_Print_Error(("BF_Play_Single_Manage(): Playback NOT_SUPPORT_ERROR\n"));
#endif  /* FJ_DEBUG_PRINT_ON */
		}
		else {
			/* In case of MPEG file, it already reported. */
		}
	}

	gBF_Play_Common_Display_Data.ercd            = ercd;
	gBF_Play_Common_Display_Data.rotate_index    = 0;
	gBF_Play_Common_Display_Data.play_rotate_flg = 0;
// --- REMOVE_DCF_EXIF END ---

	return ercd;
}

