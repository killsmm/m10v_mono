/**
 * @file		play_common.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_PB_CODE"

#include "os_user_custom.h"
#include "TKernel_id.h"

#include "sdram_map_cache_define.h"

#include "play_common.h"
#include "play_jpeg.h"
#include "play_attr.h"
#include "playback.h"
#include "play_macro_wrap.h"

#include "osd_general.h"
#include "osd_attr.h"
#include "mode.h"
#include "bus_arb.h"

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
#define D_BF_PLAY_COMMON_DRAW_LOAD_MAX_FILE_SIZE	(PIX_SIZ_4M)

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
T_BF_PLAY_COMMON_DISPLAY_DATA		gBF_Play_Common_Display_Data;
T_BF_PLAY_COMMON_IMAGE_ATTR			gBF_Play_Common_Image_Attr;
USHORT								gBF_Play_Common_Edit_Param[4]		= {0};
ULONG								gBF_Play_Common_Edit_Ex_Param[4]	= {0};
T_BF_PLAY_COMMON_ZOOM_PARAM			gBF_Play_Common_Crop_Save_Param		= {0};
T_BF_PLAY_COMMON_EDITIMAGE_LOAD		gBF_Play_Common_Edit_Image_Load;
T_BF_PLAY_COMMON_IMAGE_DISP_SIZE	gBF_Play_Common_Display_Size;
T_BF_PLAY_COMMON_IMAGE_WIN_EN		gBF_Play_Common_Image_Window = { E_BF_PLAY_COMMON_FUNC_DISABLE, 	// main
																	 E_BF_PLAY_COMMON_FUNC_DISABLE, 	// thumb
																	 E_BF_PLAY_COMMON_FUNC_DISABLE, 	// cls1
																	 E_BF_PLAY_COMMON_FUNC_DISABLE, 	// cls1thumb
																	 E_BF_PLAY_COMMON_FUNC_DISABLE, 	// cls2
																	 E_BF_PLAY_COMMON_FUNC_DISABLE };	// cls2thumb
static E_PB_AUTODISP				gBF_Play_Common_AutoDisp	= E_PB_AUTODISP_ON;


static USHORT						g_play_common_vd_cnt		= 0;
static UCHAR						g_play_common_vsync_num		= 1;
static E_IM_DISP_WAIT				g_play_common_vsync_time	= E_IM_DISP_WAIT_1_TIME;
static E_IM_DISP_SEL				g_play_dsip_sel				= E_IM_DISP_HDMI;

/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
/******************************/
/** Common Section(FastBoot) **/
/******************************/
//#pragma arm section code="BASEFW_BOOT_CODE"
/**
 * @brief		Specified JPEG is displayed in VCH.
 * @param		i:FJ_GR_FILE_TYPE						wFileType	: draw file type
 * @param		i:const T_BF_PLAY_COMMON_DRAW_PARAM*	draw_param	: Image draw parameter
 * @param		i:E_BF_PLAY_COMMON_EXEC_MODE			exec_mode	: draw executes mode
 * @param 		o:T_IM_JPEG_DEC_MNG*					jpgmng		: Jpeg decode parameter
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
static FJ_ERR_CODE play_common_draw_jpeg_file( FJ_GR_FILE_TYPE wFileType, const T_BF_PLAY_COMMON_DRAW_PARAM* draw_param, E_BF_PLAY_COMMON_EXEC_MODE exec_mode, T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng )
{
// --- REMOVE_DCF_EXIF BEGIN ---
	INT32					ret = 0;
	FJ_ERR_CODE				ercd;
	BOOL 					resized_flag;

	USHORT					rec_width;
	USHORT					rec_gl_width;
	USHORT					rec_lines;
	USHORT					back_width;
	USHORT					back_gl_width;
	USHORT					back_lines;
	USHORT					supportMaxWidth;
	USHORT					supportMaxHeight;
	UCHAR*					image_addr;
	ULONG					load_addr;
	ULONG					rec_addr;
	ULONG					rec_half_addr;
	ULONG					limit_size;			// Upper bound of size of reading image.

	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR		src;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR		rec;
	T_BF_PLAY_MACRO_WRAP_ADDR			dst_vga;
	T_BF_DCF_IF_IMAGE_DETAIL			img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA			load_image_data;

	/* initialize structure */
	memset( &img_detail,      0x00, sizeof(T_BF_DCF_IF_IMAGE_DETAIL) );
	memset( &load_image_data, 0x00, sizeof(T_BF_DCF_IF_LOAD_IMAGE_DATA) );

	// --------------------------- //
	//  Parameter Setting Process  //
	// --------------------------- //	
#if 0
	if(fj_get_act_sysmode() == FJ_MODE_MENU) {
		// Address setting
		load_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY((ULONG)SDRAM_ADR_MENU_AUDIO_IMAGE_LOAD);
		jpgmng->ycbcr_addr[0] = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)SDRAM_ADR_MENU_AUDIO_IMAGE_WORK );
		jpgmng->ycbcr_addr[1] = jpgmng->ycbcr_addr[0] + SDRAM_SIZ_MENU_AUDIO_IMAGE_Y;
		rec_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY((ULONG)SDRAM_ADR_MENU_AUDIO_IMAGE_REC);
		rec_half_addr = 0;
		
		// Support Size setting
		if(wFileType == FJ_GR_FILE_JPEG) {
			/* main image */
			// MIN((Maximum size of image), (Size of memory that can be used))
			supportMaxWidth  = BF_Osd_General_Get_Min(WIDTH_36M, WIDTH_3M_16_9);
			supportMaxHeight = BF_Osd_General_Get_Min(LINES_36M, LINES_3M_16_9);
		}
		else if(wFileType == FJ_GR_FILE_JPEG_THUMBNAIL) {
			/* thumbnail image */
			// MIN((Maximum size of image), (Size of memory that can be used))
			supportMaxWidth  = BF_Osd_General_Get_Min(WIDTH_THUMB, WIDTH_3M_16_9);
			supportMaxHeight = BF_Osd_General_Get_Min(LINES_THUMB, LINES_3M_16_9);
		}
		else {
			// wFileType error
			return FJ_ERR_NG;
		}
		limit_size = PIX_SIZ_3M_16_9; // Size of loading area of JPEG.
	}
	else {
#endif
		// Address setting
		load_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY(BF_Sdram_Map_Play_Get_Image_Load_Addr());
		jpgmng->ycbcr_addr[0] = M_SDRAM_MAP_COMMON_ADR_LOG2PHY((ULONG)SDRAM_ADR_PLAY_DRAW_FILE_WORK_AREA);	/* pgr0080 */
		jpgmng->ycbcr_addr[1] = jpgmng->ycbcr_addr[0] +  PIX_SIZ_PLAY_DRAW_FILE;
		rec_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY((ULONG)SDRAM_ADR_PLAY_DRAW_FILE_REC_AREA);	/* pgr0080 */
		rec_half_addr = BF_Sdram_Map_Play_Get_Half_Rec_Addr();
		
		// Support Size setting
		if(wFileType == FJ_GR_FILE_JPEG) {
			/* main image */
			// MIN((Maximum size of image), (Size of memory that can be used))
			supportMaxWidth  = BF_Osd_General_Get_Min(WIDTH_36M, WIDTH_4M);
			supportMaxHeight = BF_Osd_General_Get_Min(LINES_36M, LINES_4M);
		}
		else if(wFileType == FJ_GR_FILE_JPEG_THUMBNAIL) {
			/* thumbnail image */
			// MIN((Maximum size of image), (Size of memory that can be used))
			supportMaxWidth  = BF_Osd_General_Get_Min(WIDTH_THUMB, WIDTH_4M);
			supportMaxHeight = BF_Osd_General_Get_Min(LINES_THUMB, LINES_4M);
		}
		else {
			// wFileType error
			return FJ_ERR_NG;
		}
		limit_size = D_BF_PLAY_COMMON_DRAW_LOAD_MAX_FILE_SIZE;  // Size of loading area of JPEG.
//	}

	// ------------------- //
	//  JPEG Load Process  //
	// ------------------- //
	if(wFileType == FJ_GR_FILE_JPEG_THUMBNAIL) {
		/* get exif data */
		ret = BF_Dcf_If_Get_Exif_Info_Jpeg_Name( (FW_CHAR*)draw_param->file_name, &img_detail, &load_image_data );
		if( ret != 0) {
			BF_Debug_Print_Error(("play_common_draw_jpeg_file(): get exif info Error\n"));
			return FJ_ERR_NG;
		}
		if( img_detail.jpeg_format != E_BF_DCF_IF_JPEG_FORMAT_EXIF ){
			BF_Debug_Print_Error(("play_common_draw_jpeg_file(): image file is not supported!\n"));
			return FJ_ERR_NG;
		}
	}
	
	// NAND -> jpg_image load
	ercd = BF_Play_Common_Load_Jpeg( draw_param->file_name, load_addr, jpgmng, limit_size );
	if( ercd != FJ_ERR_OK ){
		BF_Debug_Print_Error(("play_common_draw_jpeg_file(): file Load Error\n"));
		return FJ_ERR_NG;
	}

	// --------------------- //
	//  JPEG Decode Process  //
	// --------------------- //
	image_addr = (UCHAR*)load_addr;
	if(wFileType == FJ_GR_FILE_JPEG_THUMBNAIL) {
		load_addr = (ULONG)(&image_addr[load_image_data.thumb_offset]);
	}
	jpgmng->code_addr	= load_addr;
	jpgmng->ycbcr_smpl	= E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422;
	jpgmng->mem_format	= E_BF_PLAY_MACRO_WRAP_JPG_MFMT_SEMI_PLANAR;
	jpgmng->gl_width	= BF_Play_Common_RoundUp_Bit_32(WIDTH_4M);
	jpgmng->code_size	= 0;	// no limit.

	/* decode to YCC420 directry start */
	if (BF_Play_Macro_Wrap_Decode_Jpeg(jpgmng, D_BF_PLAY_MACRO_WRAP_WAIT_TIME) != 0){
		BF_Debug_Print_Error(("play_common_draw_jpeg_file(): image Decode error!\n"));
		return FJ_ERR_NG;
	}
	
	/* Image Size Check */
	if(jpgmng->width > jpgmng->gl_width) {
		return FJ_ERR_NG;
	}
	
	if( (jpgmng->width > supportMaxWidth) || (jpgmng->lines > supportMaxHeight) ){
		BF_Debug_Print_Error(("play_common_draw_jpeg_file(): image file is not supported!\n"));
		return FJ_ERR_NG;
	}

	/* check executes mode */
	if(exec_mode == E_BF_PLAY_COMMON_EXEC_MODE_DEC_ONLY) {
		return FJ_ERR_OK;
	}

	// Set background size
	back_width		= draw_param->gl_width;
	back_lines		= draw_param->gl_lines;
	back_gl_width	= BF_Play_Common_RoundUp_Bit_32(back_width);
	
	// The specified size normality is checked.
	if( (draw_param->width <= 0) || (draw_param->lines <= 0) ) {
		if( (jpgmng->width > draw_param->gl_width) || (jpgmng->lines > draw_param->gl_lines) ) {
			// When it is larger than the LCD size, it is assumed that it makes an error.
			BF_Debug_Print_Error(("play_common_draw_jpeg_file(): image file is size error!\n"));
			return FJ_ERR_NG;
		}
		// The size of the read image is used.
		rec_width		= jpgmng->width;
		rec_lines		= jpgmng->lines;
		rec_gl_width	= BF_Play_Common_RoundUp_Bit_32(rec_width);
	}
	else {
		// The specified size is used.
		rec_width		= (USHORT)draw_param->width;
		rec_lines		= (USHORT)draw_param->lines;
		rec_gl_width	= BF_Play_Common_RoundUp_Bit_32(rec_width);
	}

	if( (draw_param->pos_x + rec_width) > draw_param->gl_width || (draw_param->pos_y + rec_lines) > draw_param->gl_lines ) {
		BF_Debug_Print_Error(("play_common_draw_jpeg_file(): Draw param Error! pos (x:%d,y:%d) size(width:%d,hight:%d) gl_size(width:%d,hight:%d)\n", draw_param->pos_x, draw_param->pos_y, draw_param->width, draw_param->lines, draw_param->gl_width, draw_param->gl_lines));
		return FJ_ERR_NG;
	}
	
	/* make background color */
	if( (rec_width != back_width) || (rec_lines != back_lines) ) {
		if(draw_param->bg_color.is_darw) {
			dst_vga.addr_a = draw_param->addr_y;
			dst_vga.addr_b = draw_param->addr_c;
			
			BF_Play_Macro_Wrap_Fill_Image(&dst_vga, 
										back_gl_width, back_width, back_lines, 
										draw_param->bg_color.y_color, draw_param->bg_color.cb_color, draw_param->bg_color.cr_color, 
										E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, 
										E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR, 
										E_BF_PLAY_MACRO_WRAP_FILL_ALL,
										E_BF_PLAY_MACRO_WRAP_USED_2CH);
		}
	}
	
	/* display addr set */
	dst_vga.addr_a = draw_param->addr_y + (back_gl_width * draw_param->pos_y)     + draw_param->pos_x;
	dst_vga.addr_b = draw_param->addr_c + (back_gl_width * draw_param->pos_y / 2) + draw_param->pos_x;
		
	if( (jpgmng->width == rec_width) && (jpgmng->lines == rec_lines) ) {
		// ----------------- //
		//  Display Process  //
		// ----------------- //
		if(jpgmng->ycbcr_smpl == E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422) {
			BF_Play_Macro_Wrap_Set_Size_Addr( &src, jpgmng, jpgmng->width, jpgmng->lines, jpgmng->gl_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_JPEGMNG );
			BF_Play_Macro_Wrap_Set_Size_Addr( &rec, &rec_addr, rec_width, rec_lines, rec_gl_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );
			BF_Play_Macro_Wrap_Rotate_Convert_Image( src, E_BF_PLAY_MACRO_WRAP_PFMT_YCC422, &rec, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420,
										   				E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF );

			BF_Play_Macro_Wrap_Copy_Image(rec.addrs.addr_a, rec.addrs.addr_b, rec_gl_width,
											dst_vga.addr_a, dst_vga.addr_b, back_gl_width,
											rec_width, rec_lines,
											E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, 
											E_BF_PLAY_MACRO_WRAP_USED_2CH);
		}
		else if(jpgmng->ycbcr_smpl == E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420) {
			BF_Play_Macro_Wrap_Copy_Image(jpgmng->ycbcr_addr[0], jpgmng->ycbcr_addr[1], jpgmng->gl_width,
											dst_vga.addr_a, dst_vga.addr_b, back_gl_width,
											rec_width, rec_lines,
											E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, 
											E_BF_PLAY_MACRO_WRAP_USED_2CH);
		}
		else {
			BF_Debug_Print_Error(("not supported YCC type!\n"));
			return FJ_ERR_NG;
		}

	}
	else {
		/* set source paramater */
		BF_Play_Macro_Wrap_Set_Size_Addr( &src, jpgmng,    jpgmng->width, jpgmng->lines, jpgmng->gl_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_JPEGMNG );
		BF_Play_Macro_Wrap_Set_Size_Addr( &rec, &rec_addr, rec_width,     rec_lines,     rec_gl_width,     E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );
		
		// Half Rec
		if (rec_half_addr != 0) {
			BF_Play_Common_Half_Convert( &src, &rec, rec_half_addr, jpgmng->ycbcr_smpl, &resized_flag );
		}

		if(jpgmng->ycbcr_smpl == E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422) {
			BF_Play_Macro_Wrap_Rotate_Convert_Image( src, E_BF_PLAY_MACRO_WRAP_PFMT_YCC422, &rec, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420,
												   		E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF );
		}
		else if(jpgmng->ycbcr_smpl == E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420) {
			BF_Play_Macro_Wrap_Rotate_Convert_Image( src, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, &rec, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420,
												   		E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF );
		}
		else {
			BF_Debug_Print_Error(("not supported YCC type!\n"));
			return FJ_ERR_NG;
		}

		// ----------------- //
		//  Display Process  //
		// ----------------- //
		BF_Play_Macro_Wrap_Copy_Image(rec.addrs.addr_a, rec.addrs.addr_b, rec_gl_width,
										dst_vga.addr_a, dst_vga.addr_b, back_gl_width,
										rec_width, rec_lines,
										E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, 
										E_BF_PLAY_MACRO_WRAP_USED_2CH);
	}
// --- REMOVE_DCF_EXIF END ---
	return FJ_ERR_OK;
}

/**
 * @brief		Set interruption flag
 * @param		None
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID play_common_set_flag_disp_vd(VOID)
{
	g_play_common_vd_cnt ++;
	if (g_play_common_vd_cnt == g_play_common_vsync_num * g_play_common_vsync_time) {
		OS_User_Set_Flg(FID_IMAGE_PLAY, D_BF_PLAY_COMMON_PB_WAIT_VD);
		g_play_common_vd_cnt = 0;
	}

	return;
}

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/
/******************************/
/**  Boot Section(FastBoot)  **/
/******************************/
//#pragma arm section code="BASEFW_BOOT_CODE"
/**
 * @brief		set playback automatic display status
 * @param		i:E_PB_AUTODISP auto_disp
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Set_Pb_AutoDisplay( E_PB_AUTODISP auto_disp )
{
	if (auto_disp <= E_PB_AUTODISP_ON) {
		gBF_Play_Common_AutoDisp = auto_disp;
		return FJ_ERR_OK;
	}
	else {
		return FJ_ERR_NG;
	}
}

/**
 * @brief		get playback automatic display status
 * @param		None
 * @return		E_PB_AUTODISP
 * @note		None
 * @attention	None
 */
E_PB_AUTODISP BF_Get_Pb_AutoDisplay( VOID )
{
	return gBF_Play_Common_AutoDisp;
}

/**
 * @brief		set playback display window
 * @param		i: FJ_MODE bMode
 * @param		i: USHORT wStartX
 * @param		i: USHORT wStartY
 * @param		i: USHORT wWidth
 * @param		i: USHORT wHeight
 * @param		i: BOOL bKeepAr
 * @return		FJ_ERR_OK: Success
 * @return		other error code: Fail
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Common_Set_DispWindow( FJ_MODE bMode, USHORT wStartX, USHORT wStartY, USHORT wWidth, USHORT wHeight, BOOL bKeepAr )
{
	switch(bMode){
		case FJ_MODE_PLAYBACK:
			if ((wStartX == WIDTH_VGA) || (wStartY == LINES_VGA) || (wWidth == 0) || (wHeight == 0)
				|| ((wStartX % 2) != 0) || ((wStartY % 2) != 0) || ((wWidth % 2) != 0) || ((wHeight % 2) != 0)) {
				BF_Debug_Print_Error(("BF_Play_Common_Set_DispWindow(): Input param error!! wStartX=%d, wStartY=%d, wHeight=%d, wHeight=%d\n",
								   wStartX, wStartY, wHeight, wHeight));
				return FJ_ERR_NG;
			}
			if (((wStartX + wWidth) > WIDTH_VGA) || ((wStartY + wHeight) > LINES_VGA)) {
				BF_Debug_Print_Error(("BF_Play_Common_Set_DispWindow(): Input param error2!! wStartX=%d, wStartY=%d, wHeight=%d, wHeight=%d\n",
								   wStartX, wStartY, wHeight, wHeight));
				return FJ_ERR_NG;
			}
			BF_Play_Attr_Set_Disp_Win_Play_Lcd_Pos_X( wStartX );
			BF_Play_Attr_Set_Disp_Win_Play_Lcd_Pos_Y( wStartY );
			BF_Play_Attr_Set_Disp_Win_Play_Lcd_Width( wWidth );
			BF_Play_Attr_Set_Disp_Win_Play_Lcd_Lines( wHeight );
			BF_Play_Attr_Set_Disp_Win_Play_Lcd_Arflg( bKeepAr );
			break;
		
		case FJ_MODE_PLAYBACK_THUMB:
			BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Pos_X( wStartX );
			BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Pos_Y( wStartY );
			BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Width( wWidth );
			BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Lines( wHeight );
			BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Arflg( bKeepAr );
			break;
		
		default:
			return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief		set playback display window
 * @param		i: E_PB_DISP_WIN_TYPE window_type
 * @param		i: T_PB_DISP_WINDOW* disp_window
 * @return		FJ_ERR_OK: Success
 * @return		other error code: Fail
 * @note		None
 * @attention	None
*/
FJ_ERR_CODE BF_Play_Common_Set_DisplayWindow( E_PB_DISP_WIN_TYPE window_type, T_PB_DISP_WINDOW* disp_window )
{
	switch (window_type) {
		case E_PB_DISP_WIN_TYPE_LCD:
			BF_Play_Attr_Set_Disp_Win_Play_Lcd_Pos_X( disp_window->pos_x );
			BF_Play_Attr_Set_Disp_Win_Play_Lcd_Pos_Y( disp_window->pos_y );
			BF_Play_Attr_Set_Disp_Win_Play_Lcd_Width( disp_window->width );
			BF_Play_Attr_Set_Disp_Win_Play_Lcd_Lines( disp_window->lines );
			BF_Play_Attr_Set_Disp_Win_Play_Lcd_Arflg( disp_window->ar_flg );

			BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Pos_X( disp_window->pos_x ); 
			BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Pos_Y( disp_window->pos_y ); 
			BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Width( disp_window->width ); 
			BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Lines( disp_window->lines ); 
			BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Arflg( disp_window->ar_flg ); 
			g_play_dsip_sel = E_IM_DISP_LCD_MIPI;
			break;
			
		case E_PB_DISP_WIN_TYPE_HDMI:
			BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Pos_X( disp_window->pos_x );
			BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Pos_Y( disp_window->pos_y );
			BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Width( disp_window->width );
			BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Lines( disp_window->lines );
			BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Arflg( disp_window->ar_flg );

			BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Pos_X( disp_window->pos_x ); 
			BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Pos_Y( disp_window->pos_y ); 
			BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Width( disp_window->width ); 
			BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Lines( disp_window->lines ); 
			BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Arflg( disp_window->ar_flg ); 
			g_play_dsip_sel = E_IM_DISP_HDMI;
			break;
			
		default :
			return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief		set playback image window
 * @param		i: E_PB_DISP_IMAGE disp_image
 * @param		i: T_PB_IMAGE_WINDOW* image_window
 * @return		FJ_ERR_OK: Success
 * @return		other error code: Fail
 * @note		None
 * @attention	None
*/
FJ_ERR_CODE BF_Play_Common_Set_ImageWindow( E_PB_DISP_IMAGE disp_image, T_PB_IMAGE_WINDOW* image_window )
{
	switch (disp_image) {
		case DISP_IMAGE_MAIN :
			BF_Play_Attr_Set_Image_Window_Main( image_window );
			gBF_Play_Common_Image_Window.image_win_main = E_BF_PLAY_COMMON_FUNC_ENABLE;
			break;
			
		case DISP_IMAGE_THUMB :
			BF_Play_Attr_Set_Image_Window_Thumb( image_window );
			gBF_Play_Common_Image_Window.image_win_thumb = E_BF_PLAY_COMMON_FUNC_ENABLE;
			break;
			
		case DISP_IMAGE_MPCLS1 :
			BF_Play_Attr_Set_Image_Window_CLS1( image_window );
			gBF_Play_Common_Image_Window.image_win_cls1 = E_BF_PLAY_COMMON_FUNC_ENABLE;
			break;
			
		case DISP_IMAGE_MPCLS2 :
			BF_Play_Attr_Set_Image_Window_CLS2( image_window );
			gBF_Play_Common_Image_Window.image_win_cls2 = E_BF_PLAY_COMMON_FUNC_ENABLE;
			break;
			
		default :
			return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief		cancel playback image window
 * @param		None
 * @return		FJ_ERR_OK: Success
 * @return		other error code: Fail
 * @note		None
 * @attention	None
*/
FJ_ERR_CODE BF_Play_Common_Can_ImageWindow( VOID )
{
	gBF_Play_Common_Image_Window.image_win_main      = E_BF_PLAY_COMMON_FUNC_DISABLE;
	gBF_Play_Common_Image_Window.image_win_thumb     = E_BF_PLAY_COMMON_FUNC_DISABLE;
	gBF_Play_Common_Image_Window.image_win_cls1      = E_BF_PLAY_COMMON_FUNC_DISABLE;
	gBF_Play_Common_Image_Window.image_win_cls1thumb = E_BF_PLAY_COMMON_FUNC_DISABLE;
	gBF_Play_Common_Image_Window.image_win_cls2      = E_BF_PLAY_COMMON_FUNC_DISABLE;
	gBF_Play_Common_Image_Window.image_win_cls2thumb = E_BF_PLAY_COMMON_FUNC_DISABLE;

	return FJ_ERR_OK;
}

/**
 * @brief		aspect ratio(4:3,16:9,3:2,5:4,3:4,9:16,2:3,4:5,1:1) is decided 
 * @param		i:width			width size
 * @param		i:lines			lines size
 * @param		o:asp_type		(0=NG, 1=4:3, 2=16:9, 3=3:2, 4=5:4, 5=3:4, 6=9:16, 7=2:3, 8=4:5, 9=1:1)
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Common_Get_Aspect_Type( USHORT width, USHORT lines, E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE* asp_type )
{
	USHORT		size;
	FJ_ERR_CODE ercd;
	
	ercd = FJ_ERR_OK;

	// 1 : 1
	if (width == lines) {
		*asp_type = E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_1_1;
		return ercd;
	}
	
	// 4 : 3  (width*3/4)
	size = (width + (width << 1)) >> 2;
	if (size == lines) {
		*asp_type = E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_4_3;
		return ercd;
	}
	
	// 16 : 9  (width*9/16)
	size = (width + (width << 3)) >> 4;
	if (size == lines) {
		*asp_type = E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_16_9;
		return ercd;
	}
	
	/* The same computational method as the cnv_aspect_sizecnv_aspect_size() function is used. */
	// 3 : 2  (lines*3/2)
	size = (lines + (lines << 1)) >> 1;
	if (size == width) {
		*asp_type = E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_3_2;
		return ercd;
	}
	
	// 3 : 4  (lines*3/4)
	size = (lines + (lines << 1)) >> 2;
	if (size == width) {
		*asp_type = E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_3_4;
		return ercd;
	}
	
	// 9 : 16  (lines*9/16)
	size = (lines + (lines << 3)) >> 4;
	if (size == width) {
		*asp_type = E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_9_16;
		return ercd;
	}
	
	// 2 : 3  (width*3/2)
	size = (width + (width << 1)) >> 1;
	if (size == lines) {
		*asp_type = E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_2_3;
		return ercd;
	}

	BF_Play_Common_Get_Defined_Aspect_Type( width, lines, asp_type );
	
	if (*asp_type == E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_UNDEFINE) {
		BF_Debug_Print_Information(("playback aspect illigal(type)\n"));
		ercd = FJ_ERR_NG;
	}
	return ercd;

}

/**
 * @brief		The aspect ratio (It is 16:9 or 3:2) is judged from define.
 * @param		i:width			width size
 * @param		i:lines			lines size
 * @param		o:asp_type		(0=NG, 2=16:9, 3=3:2)
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Common_Get_Defined_Aspect_Type( USHORT width, USHORT lines, E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE* asp_type )
{
	/*
	 This function is a supplementary function of BF_Play_Common_Get_Aspect_Type().
	 Therefore, this function is recommended not to be used directly.
	*/
	if ((width == WIDTH_36M && lines == LINES_36M)
		|| (width == WIDTH_12M_3_2 && lines == LINES_12M_3_2)
		|| (width == WIDTH_1_2M && lines == LINES_1_2M_3_2)
		|| (width == WIDTH_VGA   && lines == LINES_VGA_3_2)
		|| (width == WIDTH_QVGA   && lines == LINES_QVGA_3_2)
		|| (width == WIDTH_THUMB && lines == LINES_THUMB_3_2)
		|| (width == FJ_CUSTOM_WIDTH_SCR && lines == FJ_CUSTOM_LINES_SCR_3_2)) {
		// 3:2
		*asp_type = E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_3_2;
	}
	else if ((width == WIDTH_12M_16_9 && lines == LINES_12M_16_9)
			|| (width == WIDTH_10M_16_9 && lines == LINES_10M_16_9)) {
		// 16:9
		*asp_type = E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_16_9;
	}
	else {
		// 4:3 and the rest.
		*asp_type = E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_UNDEFINE;
	}
	return;
}

/******************************/
/** Common Section(FastBoot) **/
/******************************/
//#pragma arm section code="BASEFW_COMM_CODE"
/**
 * @brief		reset display data for after file delete operation
 * @param		None
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Common_Reset_Current_Data( VOID )
{
// --- REMOVE_DCF_EXIF BEGIN ---
	gBF_Play_Common_Display_Data.src_addr             = 0;
	gBF_Play_Common_Display_Data.rotate_index         = 0;
	gBF_Play_Common_Display_Data.current_rotate       = E_BF_DCF_IF_ROTATE_0;
	gBF_Play_Common_Display_Data.play_rotate          = E_BF_DCF_IF_ROTATE_0;
	gBF_Play_Common_Display_Data.current_rotate_thumb = E_BF_DCF_IF_ROTATE_0;
	gBF_Play_Common_Display_Data.play_rotate_thumb    = E_BF_DCF_IF_ROTATE_0;
	gBF_Play_Common_Display_Data.jpeg_format          = E_BF_DCF_IF_JPEG_FORMAT_EXIF;
	gBF_Play_Common_Display_Data.play_file_info       = E_BF_PLAY_COMMON_FILE_INFO_NOKIND;
	gBF_Play_Common_Display_Data.ercd                 = FJ_ERR_NG;
	gBF_Play_Common_Display_Data.play_rotate_flg      = 0;
	gBF_Play_Common_Display_Data.panorama             = 0;
	
	
	BF_Exif_Reset_Index();

	return;
// --- REMOVE_DCF_EXIF END ---
}

//#pragma arm section code="BASEFW_BOOT_CODE"

/**
 * @brief		JPEG load function
 * @param		i:CHAR* file_name
 * @param		i:ULONG load_addr
 * @param		o:T_IM_JPEG_DEC_MNG* jpgmng
 * @param		i:ULONG limit_size
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Common_Load_Jpeg( CHAR* file_name, ULONG load_addr, T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng, ULONG limit_size )
{
	T_BF_FS_IF_STAT		fsif_stat;
	INT32				file_no;
	FW_ULONG			length = 0;

	if(	( BF_Fs_If_Stat( (FW_CHAR*)file_name, &fsif_stat ) != D_BF_FS_USER_CUSTOM_OK ) ) {
		return FJ_ERR_NG;
	}
	/* file size chk */
	if( fsif_stat.fileSize > limit_size ){
		BF_Debug_Print_Error(("BF_Play_Common_Load_Jpeg(): file size error! size:%llu\n", fsif_stat.fileSize ));
		return FJ_ERR_NG;
	}
	file_no = BF_Fs_If_Open( (FW_CHAR*)file_name, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_READONLY );

	if( file_no != 0 ) {
		if( BF_Fs_If_Read( file_no, (UCHAR*)load_addr, fsif_stat.fileSize, &length ) != 0 ) {
			BF_Debug_Print_Error(("BF_Play_Common_Load_Jpeg(): file read error!\n"));
			BF_Fs_If_Close( file_no );
			return FJ_ERR_NG;
		}
		jpgmng->code_size = length;
		BF_Fs_If_Close( file_no );
	}
	else {
		return FJ_ERR_NG;
	}
	return FJ_ERR_OK;
}

/**
 * @brief		JPEG draw function
 * @param		i:FJ_GR_FILE_TYPE				wFileType
 * @param		i:T_BF_PLAY_COMMON_DRAW_PARAM	draw_param
 * @param		i:E_BF_PLAY_COMMON_EXEC_MODE	exec_mode
 * @param		o:T_IM_JPEG_DEC_MNG*			jpgmng
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Common_Draw_Jpeg( FJ_GR_FILE_TYPE wFileType, const T_BF_PLAY_COMMON_DRAW_PARAM* draw_param, E_BF_PLAY_COMMON_EXEC_MODE exec_mode, T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpgmng )
{
	FJ_ERR_CODE	ercd;

	if( (draw_param->width <= 32) || (draw_param->lines <= 24) ) {
		BF_Debug_Print_Error(("BF_Play_Common_Draw_Jpeg(): Draw param Error! pos (x:%d,y:%d) size(width:%d,hight:%d)\n", draw_param->pos_x, draw_param->pos_y, draw_param->width, draw_param->lines));
		return FJ_ERR_NG;
	}

	switch(wFileType) {
		case FJ_GR_FILE_JPEG:			// FALLTHROUGH
		case FJ_GR_FILE_JPEG_THUMBNAIL:	// FALLTHROUGH
		case FJ_GR_FILE_JPEG_QVGA:		// FALLTHROUGH
		case FJ_GR_FILE_SCREENNAIL:
			// Draw File
			ercd = play_common_draw_jpeg_file(wFileType, draw_param, exec_mode, jpgmng);
			break;
		default:
			// Not support
			ercd = FJ_ERR_NG;
			break;
	}
	return ercd;
}

/******************************/
/**  PB Section(FastBoot)    **/
/******************************/
//#pragma arm section code="BASEFW_PB_CODE"

// --- REMOVE_DCF_EXIF BEGIN ---
/**
 * @brief		rotate convert
 * @param		i:E_BF_DCF_IF_ROTATE src_rotate
 * @param		o:E_BF_DCF_IF_ROTATE dst_rotate
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Common_Rotate_Check_Exif_Tag( E_BF_DCF_IF_ROTATE src_rotate, E_BF_DCF_IF_ROTATE* dst_rotate )
{
	/* rotate mirror convert rotate = 0 */
	switch(src_rotate) {
		case E_BF_DCF_IF_ROTATE_0:	// FALL THROUGH
		case E_BF_DCF_IF_ROTATE_180:	// FALL THROUGH
		case E_BF_DCF_IF_ROTATE_90:	// FALL THROUGH
		case E_BF_DCF_IF_ROTATE_270:
		case E_BF_DCF_IF_ROTATE_MIRROR_0:
		case E_BF_DCF_IF_ROTATE_MIRROR_180:
		case E_BF_DCF_IF_ROTATE_MIRROR_90:
		case E_BF_DCF_IF_ROTATE_MIRROR_270:
			*(dst_rotate) = src_rotate;
			break;

		default:
			*(dst_rotate) = E_BF_DCF_IF_ROTATE_0;
			break;
	}

}
// --- REMOVE_DCF_EXIF END ---

/**
 * @brief		Image half rec.
 * @param		i/o:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR*	src						src data
 * @param		i  :T_BF_PLAY_MACRO_WRAP_SIZE_ADDR*	dst						dst data
 * @param		i  :ULONG							half_rec_addr half		rec addr
 * @param		i  :UCHAR							ycc_smpl				YCC type
 * @param		i/o:BOOL* 							resized_flag			HDMI has resized flag
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Common_Half_Convert( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* src, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* dst, ULONG half_rec_addr, UCHAR ycc_smpl, BOOL* resized_flag )
{
	INT32	ret = -1;
	CHAR	count = 0;
	CHAR	class2_rec_flg = 1;
	USHORT	gl_half_width = 0;
	USHORT	src_size_width = 0;
	USHORT	src_size_lines = 0;
	USHORT	dst_size_width = 0;
	USHORT	dst_size_lines = 0;	
	USHORT	dst_size_width_vga = 0;
	USHORT	dst_size_lines_vga = 0;
	USHORT	asp_size_lines = 0;
	T_BF_PLAY_MACRO_WRAP_ADDR			rec_addr;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR		half;
	E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE	asp_type;
	E_BF_PLAY_MACRO_WRAP_PFMT 			ycc_pfmt;

	memset( &half, 0x00, sizeof(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR) );

	BF_Play_Common_Get_Aspect_Type(src->size.width, src->size.lines, &asp_type);
	BF_Play_Common_Change_YCC_Smpl_To_Pfmt( (E_BF_PLAY_MACRO_WRAP_JPG_SMPL)ycc_smpl, &ycc_pfmt );
	
	if(half_rec_addr != ((ULONG)NULL)) {
		switch(asp_type) {
			case E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_4_3:
				dst_size_width     = WIDTH_FHD_4_3;
				dst_size_lines     = LINES_FHD;
				dst_size_width_vga = WIDTH_VGA;
				dst_size_lines_vga = LINES_VGA;
				break;
			case E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_3_2:
				dst_size_width     = WIDTH_FHD_3_2;
				dst_size_lines     = LINES_FHD;
				dst_size_width_vga = WIDTH_VGA;
				dst_size_lines_vga = LINES_VGA_3_2;
				break;
			case E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_16_9:
				dst_size_width     = WIDTH_FHD;
				dst_size_lines     = LINES_FHD;
				dst_size_width_vga = WIDTH_VGA;
				dst_size_lines_vga = LINES_VGA_16_9;
				break;
			default:
				class2_rec_flg     = 0;
				break;
		}
		if(!class2_rec_flg) {
			asp_size_lines = ((src->size.width * 3) >> 2);
			if( ((asp_size_lines + 1) == src->size.lines) || ((asp_size_lines - 1) == src->size.lines) ) {
				dst_size_width     = WIDTH_FHD_4_3;
				dst_size_lines     = LINES_FHD;
				dst_size_width_vga = WIDTH_VGA;
				dst_size_lines_vga = LINES_VGA;
				class2_rec_flg     = 1;
				asp_type           = E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_4_3;
			}
			else {
				asp_size_lines = ((src->size.width * 2) / 3);
				if( ((asp_size_lines + 1) == src->size.lines) || ((asp_size_lines - 1) == src->size.lines) ) {
					dst_size_width     = WIDTH_FHD_3_2;
					dst_size_lines     = LINES_FHD;
					dst_size_width_vga = WIDTH_VGA;
					dst_size_lines_vga = LINES_VGA_3_2;
					class2_rec_flg     = 1;
					asp_type           = E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_3_2;
				}
				else {
					asp_size_lines = ((src->size.width * 9) >> 4);
					if( ((asp_size_lines + 1) == src->size.lines) || ((asp_size_lines - 1) == src->size.lines) ) {
						dst_size_width     = WIDTH_FHD;
						dst_size_lines     = LINES_FHD;
						dst_size_width_vga = WIDTH_VGA;
						dst_size_lines_vga = LINES_VGA_16_9;
						class2_rec_flg     = 1;
						asp_type           = E_BF_PLAY_MACRO_WRAP_ASPECT_TYPE_16_9;
					}
				}
			}
		}

		for(count = D_BF_PLAY_COMMON_HALF_REC_COUNT; count > 0; count--) {
			if( ((src->size.width >> 1) > dst->size.width) || ((src->size.lines >> 1) > dst->size.lines) ) {
				if( ((src->size.width >> 1) > dst_size_width) || ((src->size.lines >> 1) > dst_size_lines) ) {
					src_size_width = (src->size.width >> 1);
					src_size_lines = (src->size.lines >> 1);
				}
				else if( ((src->size.width > dst_size_width) || (src->size.lines > dst_size_lines)) && (gBF_Play_Common_Display_Data.panorama == 0) && (class2_rec_flg != 0) ){
					src_size_width = dst_size_width;
					src_size_lines = dst_size_lines;
				}
				else if( ((src->size.width >> 1) > dst_size_width_vga) || ((src->size.lines >> 1) > dst_size_lines_vga) ) {
					src_size_width = src->size.width >> 1;
					src_size_lines = src->size.lines >> 1;
				}
				else if( ((src->size.width > dst_size_width_vga) || (src->size.lines > dst_size_lines_vga)) && (gBF_Play_Common_Display_Data.panorama == 0) && (class2_rec_flg != 0) ){
					src_size_width = dst_size_width_vga;
					src_size_lines = dst_size_lines_vga;
				}
				else {
					src_size_width = (src->size.width >> 1);
					src_size_lines = (src->size.lines >> 1);
				}

				src_size_width = BF_Play_Common_RoundDown_Bit_2(src_size_width);
				src_size_lines = BF_Play_Common_RoundDown_Bit_2(src_size_lines);
				gl_half_width  = BF_Play_Common_RoundUp_Bit_32(src_size_width);

				if (count == D_BF_PLAY_COMMON_HALF_REC_COUNT) {
					if(!(*resized_flag)) {
						rec_addr.addr_a = half_rec_addr;
						rec_addr.addr_b = rec_addr.addr_a + (SDRAM_ADR_PLAY_IMAGE_HALF_REC_AREA_0_C - SDRAM_ADR_PLAY_IMAGE_HALF_REC_AREA_0_Y);
					}
					else {
						rec_addr.addr_a = src->addrs.addr_a;
						rec_addr.addr_b = src->addrs.addr_b;
						rec_addr.addr_a -= src_size_width * 8;
						rec_addr.addr_a = ((rec_addr.addr_a >> 5) << 5);
						rec_addr.addr_b -= src_size_width * 8;
						rec_addr.addr_b = ((rec_addr.addr_b >> 5) << 5);
					}
				}
				else { //count != D_BF_PLAY_COMMON_HALF_REC_COUNT
					rec_addr.addr_a -= src_size_width * 8;	/* pgr0872 */	/* pgr0000 */
					rec_addr.addr_a = ((rec_addr.addr_a >> 5) << 5);
					rec_addr.addr_b -= src_size_width * 8;	/* pgr0872 */
					rec_addr.addr_b = ((rec_addr.addr_b >> 5) << 5);
				}
				
				BF_Play_Macro_Wrap_Set_Size_Addr( &half, &rec_addr, src_size_width, src_size_lines, gl_half_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR );
// --- REMOVE_DCF_EXIF BEGIN ---
				ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( *src, ycc_pfmt, &half, ycc_pfmt, E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF );
// --- REMOVE_DCF_EXIF END ---
				if (ret != 0) {
					return FJ_ERR_NG;
				}
				
				memcpy(src, &half, sizeof(half));
				*resized_flag = TRUE;
			}
			else {
				break;
			}
		}
	}

	return FJ_ERR_OK;
}

// --- REMOVE_DCF_EXIF BEGIN ---
/**
 * @brief		JPEG data is converted into display device size of the YCC420 format.
 * @param		i:T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG*	jpg_dec_mng		JPEG decode DATA
 * @param		o:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* 		hd_param		Destination image for HD
 * @param		o:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* 		vga_param		Destination image for VGA
 * @param		i:ULONG									half_addr		Half resize area address
 * @param		i:E_BF_DCF_IF_ROTATE 					rotate			Image rotate
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Common_Convert_2_Display( T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG* jpg_dec_mng, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* hd_param, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* vga_param, ULONG half_addr, E_BF_DCF_IF_ROTATE rotate )
{
	INT32	ret = 0;
	UCHAR	effective_device;
	BOOL	resized_flag = FALSE;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR		src_param;
	T_BF_PLAY_MACRO_WRAP_SIZE			output_hd_size;

	if (gBF_Play_Common_Image_Window.image_win_main == E_BF_PLAY_COMMON_FUNC_ENABLE) {
	// Enable the image window
		T_PB_IMAGE_WINDOW				image_window;
		T_BF_PLAY_MACRO_WRAP_ADDR		src_addr;

		// Get image window parameter
		BF_Play_Attr_Get_Image_Window_Main( &image_window );

		// Set Y address
		src_addr.addr_a = jpg_dec_mng->ycbcr_addr[0] + (jpg_dec_mng->gl_width * image_window.pos_y) + image_window.pos_x;

		switch (jpg_dec_mng->ycbcr_smpl) {
			case E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420:
				// Set C address
				src_addr.addr_b = jpg_dec_mng->ycbcr_addr[1] + (jpg_dec_mng->gl_width * (image_window.pos_y >> 1)) + image_window.pos_x;
				break;
				
			case E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422:
				// Set C address
				src_addr.addr_b = jpg_dec_mng->ycbcr_addr[1] + (jpg_dec_mng->gl_width * image_window.pos_y) + image_window.pos_x;
				break;
				
			default:
				BF_Debug_Print_Error(("BF_Play_Common_Convert_2_Display() : Unsupported sampling type error.\n"));
				return FJ_ERR_NG;
		}

		// Set source address & size
		BF_Play_Macro_Wrap_Set_Size_Addr( &src_param, &src_addr, image_window.width, image_window.lines, jpg_dec_mng->gl_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR );
	}
	else {
		// Set source address & size
		BF_Play_Macro_Wrap_Set_Size_Addr( &src_param, jpg_dec_mng, jpg_dec_mng->width, jpg_dec_mng->lines, jpg_dec_mng->gl_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_JPEGMNG );
	}

	effective_device = BF_Play_Macro_Wrap_Get_Effective_Device();	// LCD/HD/TV connect status get
	
	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_HDMI & effective_device) {		// HDMI enable
		// Get output image size of HDMI
		if (BF_Play_Macro_Wrap_Get_Hdmi_Output_Size( &output_hd_size ) != FJ_ERR_OK) {
			BF_Debug_Print_Error(("BF_Play_Common_Convert_2_Display(): BF_Play_Macro_Wrap_Get_Hdmi_Output_Size error\n"));
			return FJ_ERR_NG;
		}
		
		// Set hd destination size
		hd_param->size.width = output_hd_size.width;
		hd_param->size.lines = output_hd_size.lines;
		
		// Half Rec
		ret = BF_Play_Common_Half_Convert( &src_param, hd_param, half_addr, jpg_dec_mng->ycbcr_smpl, &resized_flag );
		if (ret != 0) {
			BF_Debug_Print_Error(("BF_Play_Common_Convert_2_Display(): BF_Play_Common_Half_Convert error.\n"));
			return FJ_ERR_NG;
		}

		if (jpg_dec_mng->ycbcr_smpl == E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420) {
		// YCC420
			if ((jpg_dec_mng->width == output_hd_size.width) && (jpg_dec_mng->lines == output_hd_size.lines) && (rotate == E_BF_DCF_IF_ROTATE_0)) {
			// HD size
				// Copy YCC image data
				BF_Play_Macro_Wrap_Copy_Image(src_param.addrs.addr_a, src_param.addrs.addr_b, src_param.size.global_width, 
												hd_param->addrs.addr_a, hd_param->addrs.addr_b, hd_param->size.global_width, 
												hd_param->size.width, hd_param->size.lines,
												E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, 
												E_BF_PLAY_MACRO_WRAP_USED_2CH);
			}
			else {
			// Not HD size
				ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( src_param, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, hd_param, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, rotate, E_BF_PLAY_MACRO_WRAP_ADJUST_ON );
				if (ret != 0) {
					BF_Debug_Print_Error(("BF_Play_Common_Convert_2_Display(): BF_Play_Macro_Wrap_Rotate_Convert_Image error.\n"));
					return FJ_ERR_NG;
				}
			}
		}
		else {
			// Convert to YCC420
			ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( src_param, E_BF_PLAY_MACRO_WRAP_PFMT_YCC422, hd_param, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, rotate, E_BF_PLAY_MACRO_WRAP_ADJUST_ON );
			if (ret != 0) {
				BF_Debug_Print_Error(("BF_Play_Common_Convert_2_Display(): BF_Play_Macro_Wrap_Rotate_Convert_Image error.\n"));
				return FJ_ERR_NG;
			}
		}
	}
	
	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_LCD & effective_device) {		// LCD enable
		// Half Rec
		ret = BF_Play_Common_Half_Convert( &src_param, vga_param, half_addr, jpg_dec_mng->ycbcr_smpl, &resized_flag );
		if (ret != 0) {
			BF_Debug_Print_Error(("BF_Play_Common_Convert_2_Display(): BF_Play_Common_Half_Convert error.\n"));
			return FJ_ERR_NG;
		}

		if (jpg_dec_mng->ycbcr_smpl == E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420) {
		// YCC420
			if ((jpg_dec_mng->width == WIDTH_VGA) && (jpg_dec_mng->lines == LINES_VGA) && (rotate == E_BF_DCF_IF_ROTATE_0)) {
			// VGA size
				// Copy YCC image data
				BF_Play_Macro_Wrap_Copy_Image(src_param.addrs.addr_a, src_param.addrs.addr_b, src_param.size.global_width, 
												vga_param->addrs.addr_a, vga_param->addrs.addr_b, vga_param->size.global_width, 
												vga_param->size.width, vga_param->size.lines,
												E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, 
												E_BF_PLAY_MACRO_WRAP_USED_2CH);
			}
			else {
			// Not VGA size
				ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( src_param, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, vga_param, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, rotate, E_BF_PLAY_MACRO_WRAP_ADJUST_ON );
				if (ret != 0) {
					BF_Debug_Print_Error(("BF_Play_Common_Convert_2_Display(): BF_Play_Macro_Wrap_Rotate_Convert_Image error.\n"));
					return FJ_ERR_NG;
				}
			}
		}
		else {
			// Convert to YCC420
			ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( src_param, E_BF_PLAY_MACRO_WRAP_PFMT_YCC422, vga_param, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, rotate, E_BF_PLAY_MACRO_WRAP_ADJUST_ON );
			if (ret != 0) {
				BF_Debug_Print_Error(("BF_Play_Common_Convert_2_Display(): BF_Play_Macro_Wrap_Rotate_Convert_Image error.\n"));
				return FJ_ERR_NG;
			}
		}
	}

	return FJ_ERR_OK;
}
// --- REMOVE_DCF_EXIF END ---

/**
 * @brief		JPEG data is converted data chk.
 * @param		i:INT32 index		Target file index
 * @return 		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Common_Check_Load_File_Size_Index( INT32 index )
{
// --- REMOVE_DCF_EXIF BEGIN ---
	T_BF_FS_IF_STAT fsif_stat;

	// The size of the file that does LOAD is acquired.
	if(BF_Dcf_If_Fstat(index, &fsif_stat) != FSDCF_ERRNO_OK) {
		BF_Debug_Print_Error(("BF_Play_Common_Check_Load_File_Size_Index(): BF_Dcf_If_Fstat returned err.\n"));
		return FJ_ERR_NG;
	}
	
	// The upper bound of the size of the file that does LOAD is checked. 
	if(fsif_stat.fileSize > D_BF_PLAY_JPEG_LOAD_MAX_FILE_SIZE) {
		BF_Debug_Print_Error(("BF_Play_Common_Check_Load_File_Size_Index(): The size of the file is too large.\n"));
		return FJ_ERR_NG;
	}
// --- REMOVE_DCF_EXIF END ---

	return FJ_ERR_OK;
}

/**
 * @brief		JPEG data is converted data chk.
 * @param		i:FW_CHAR* file_path		Target file Path
 * @return 		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Common_Check_Load_File_Size_Path( FW_CHAR* file_path )
{
	T_BF_FS_IF_STAT fsif_stat;

	// The size of the file that does LOAD is acquired.
	if(BF_Fs_If_Stat(file_path, &fsif_stat) != FSDCF_ERRNO_OK) {
		BF_Debug_Print_Error(("Play_Jpeg_Load_Size_Check_By_FilePath(): BF_Fs_If_Stat returned err.\n"));
		return FJ_ERR_NG;
	}
	// The upper bound of the size of the file that does LOAD is checked.
	if(fsif_stat.fileSize > D_BF_PLAY_JPEG_LOAD_MAX_FILE_SIZE) {
		BF_Debug_Print_Error(("Play_Jpeg_Load_Size_Check_By_FilePath(): The size of the file is too large.\n"));
		return FJ_ERR_NG;
	}

	return FJ_ERR_OK;
}

/**
 * @brief		JPEG main data load end callback process to Play mode
 * @param		i:INT32 result
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Common_Load_Result_Entry( INT32 result )
{
	OS_User_Set_Flg(FID_IMAGE_PLAY, D_BF_PLAY_COMMON_MAIN_LOADENG);
	return;
}

/**
 * @brief		JPEG main data DCF load end process to Play mode
 * @param		i:INT32* ercd
 * @param		i:ULONG* length
 * @return		loading status
 * @note		None
 * @attention	None
 */
INT32 BF_Play_Common_Load_End( INT32* ercd, ULONG* length )
{
	T_OS_USER_RFLG pk_rflg;

	if(ercd == NULL){
		return D_BF_PLAY_COMMON_DIVLOAD_END;
	}

	*ercd = OS_User_Ref_Flg(FID_IMAGE_PLAY, &pk_rflg);
	if (*ercd != 0){
#ifdef FJ_DEBUG_PRINT_ON
	BF_Debug_Print_Error(("BF_Play_Common_Load_End(): ERROR\n"));
#endif	/* FJ_DEBUG_PRINT_ON */
		return D_BF_PLAY_COMMON_DIVLOAD_END;
	}

	if ((pk_rflg.flgptn & D_BF_PLAY_COMMON_MAIN_LOADENG) != D_BF_PLAY_COMMON_MAIN_LOADENG){
		return D_BF_PLAY_COMMON_DIVLOAD_CONTINUE;
	}

	return D_BF_PLAY_COMMON_DIVLOAD_END;
}

/**
 * @brief		Check JPEG file
 * @param		None
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Common_Check_Jpeg_File()
{
// --- REMOVE_DCF_EXIF BEGIN ---
	FW_INT32	ret;
	T_BF_DCF_IF_FILE_INFO	file_info;

	/********************************************************/
	/* confirmation of file type (JPEG file only supported) */
	/********************************************************/
	ret = BF_Dcf_If_Get_File_Info( BF_Play_Attr_Get_Image_Index(), &file_info );
	if (ret != D_OS_USER_E_OK) {
		BF_Debug_Print_Error(("BF_Play_Common_Check_Jpeg_File(): BF_Dcf_If_Get_File_Info err. ret=%x\n", ret));
		return FJ_ERR_NG;
	}

	if (!BF_Dcf_If_Is_File_Type("JPG", file_info.file_type)) {
		BF_Debug_Print_Error(("BF_Play_Common_Check_Jpeg_File(): Current file is not supported!\n"));
		return FJ_ERR_NG;
	}
	
// --- REMOVE_DCF_EXIF END ---
	return FJ_ERR_OK;
}

/**
 * @brief		E_BF_PLAY_MACRO_WRAP_PFMT ycc to E_IM_JPEG_SMPL_TYPE ycc
 * @param		i: E_BF_PLAY_MACRO_WRAP_PFMT src_ycc_type
 * @param		o: E_IM_JPEG_SMPL_TYPE* dst_ycc_type
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Common_Change_YCC_Pfmt_To_Smpl( E_BF_PLAY_MACRO_WRAP_PFMT src_ycc_type, E_BF_PLAY_MACRO_WRAP_JPG_SMPL* dst_ycc_type )
{
	switch(src_ycc_type) {
		case E_BF_PLAY_MACRO_WRAP_PFMT_YCC444:
			*dst_ycc_type = E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC444;
			break;
			
		case E_BF_PLAY_MACRO_WRAP_PFMT_YCC422:
			*dst_ycc_type = E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422;
			break;
			
		case E_BF_PLAY_MACRO_WRAP_PFMT_YCC420:
			*dst_ycc_type = E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420;
			break;

		default:
			*dst_ycc_type = E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420;
			break;
	}

	return;
}

/**
 * @brief		E_IM_JPEG_SMPL_TYPE ycc to E_BF_PLAY_MACRO_WRAP_PFMT ycc
 * @param		i: E_IM_JPEG_SMPL_TYPE src_ycc_type
 * @param		o: E_BF_PLAY_MACRO_WRAP_PFMT* dst_ycc_type
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Common_Change_YCC_Smpl_To_Pfmt( E_BF_PLAY_MACRO_WRAP_JPG_SMPL src_ycc_type, E_BF_PLAY_MACRO_WRAP_PFMT* dst_ycc_type )
{
	switch(src_ycc_type) {
		case E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC444:
			*dst_ycc_type = E_BF_PLAY_MACRO_WRAP_PFMT_YCC444;
			break;
			
		case E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422:
			*dst_ycc_type = E_BF_PLAY_MACRO_WRAP_PFMT_YCC422;
			break;
			
		case E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420:
			*dst_ycc_type = E_BF_PLAY_MACRO_WRAP_PFMT_YCC420;
			break;

		default:
			*dst_ycc_type = E_BF_PLAY_MACRO_WRAP_PFMT_NONE;
			break;
	}

	return;
}

/**
 * @brief		Set callback of interrupt of Disp Macro.
 * @param		i: BOOL bSetup
 * @param		i: UCHAR vsync_num
 * @param		i: E_IM_DISP_WAIT vsync_time
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Common_Disp_Set_Int_Callback(BOOL bSetup, UCHAR vsync_num, E_IM_DISP_WAIT vsync_time)
{
	g_play_common_vsync_num  = vsync_num;
	g_play_common_vsync_time = vsync_time;
	g_play_common_vd_cnt     = 0;
	INT32 ret = D_DDIM_OK;
	
	/* event flg clear */
	OS_User_Clr_Flg( FID_IMAGE_PLAY, ~D_BF_PLAY_COMMON_PB_WAIT_VD );

	if (bSetup){
		ret = Im_DISP_Set_Int_Callback(g_play_dsip_sel, E_IM_DISP_INTERRUPTION_SELECT_VE, play_common_set_flag_disp_vd);
		if ( ret != D_DDIM_OK ){
			BF_Debug_Print_Error(("Im_DISP_Set_Int_Callback error=%08X\n", ret));
		}
	}
	else {
		ret = Im_DISP_Set_Int_Callback(g_play_dsip_sel, E_IM_DISP_INTERRUPTION_SELECT_VE, NULL);
		if ( ret != D_DDIM_OK ){
			BF_Debug_Print_Error(("Im_DISP_Set_Int_Callback error=%08X\n", ret));
		}
	}

	return;
}

/**
 * @brief		wait flag of interpret of Disp Macro.
 * @return		None
 * @note		None
 * @attention	None
 */
VOID BF_Play_Common_DISP_Wait_Vsync(VOID)
{
	OS_USER_FLGPTN flgptn;

	OS_User_Twai_Flg( FID_IMAGE_PLAY, D_BF_PLAY_COMMON_PB_WAIT_VD, D_OS_USER_TWF_ORW, &flgptn, D_BF_PLAY_MACRO_WRAP_WAIT_TIME );
	
	return;
}

/**
 * @brief		Set Bus Arbitration
 * @param		None
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Common_Set_BusArbitration( VOID )
{
	FJ_ERR_CODE fj_ercd = FJ_ERR_OK;
	E_FJ_BUS_ARB_MXIC_UNIT unit;
	static const T_FJ_BUS_ARB_MXIC_OUTPUT_PORT output_port = {	// Tentative
		.w_out_port = {
			.ela	= E_FJ_BUS_ARB_MXIC_PORT_GR_0,
			.xch	= E_FJ_BUS_ARB_MXIC_PORT_GR_0,
			.fpt_0	= E_FJ_BUS_ARB_MXIC_PORT_GR_0,
			.fpt_1	= E_FJ_BUS_ARB_MXIC_PORT_GR_0,
		},
		.r_out_port = {
			.ela	= E_FJ_BUS_ARB_MXIC_PORT_GR_0,
			.xch	= E_FJ_BUS_ARB_MXIC_PORT_GR_0,
			.fpt_0	= E_FJ_BUS_ARB_MXIC_PORT_GR_0,
			.fpt_1	= E_FJ_BUS_ARB_MXIC_PORT_GR_0,
		},
	};
	static const T_FJ_BUS_ARB_MXIC_ALL_ARBITER_ASSIGN arbiter_assign = {	// Tentative
		.w_arbiter[E_FJ_BUS_ARB_MXIC_W_ARBITER_W1] = {	// SDRAM Controller 0 Write channel
			.port[E_FJ_BUS_ARB_MXIC_PORT_0] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_1] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_2] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
			},
		},
		.w_arbiter[E_FJ_BUS_ARB_MXIC_W_ARBITER_W2] = {	// SDRAM Controller 1 Write channel
			.port[E_FJ_BUS_ARB_MXIC_PORT_0] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_1] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_2] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
			},
		},
		.w_arbiter[E_FJ_BUS_ARB_MXIC_W_ARBITER_W3] = {	// SDRAM Controller 2 Write channel
			.port[E_FJ_BUS_ARB_MXIC_PORT_0] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_1] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_2] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
			},
		},
		.w_arbiter[E_FJ_BUS_ARB_MXIC_W_ARBITER_W4] = {	// Image-SRAM Controller Write channel
			.port[E_FJ_BUS_ARB_MXIC_PORT_0] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_1] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_2] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
					D_FJ_BUS_ARB_MXIC_W_RESERVE,
				},
			},
		},
		.r_arbiter[E_FJ_BUS_ARB_MXIC_R_ARBITER_R1] = {	// SDRAM Controller 0 Read channel
			.port[E_FJ_BUS_ARB_MXIC_PORT_0] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_1] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_2] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
			},
		},
		.r_arbiter[E_FJ_BUS_ARB_MXIC_R_ARBITER_R2] = {	// SDRAM Controller 1 Read channel
			.port[E_FJ_BUS_ARB_MXIC_PORT_0] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_1] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_2] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
			},
		},
		.r_arbiter[E_FJ_BUS_ARB_MXIC_R_ARBITER_R3] = {	// SDRAM Controller 2 Read channel
			.port[E_FJ_BUS_ARB_MXIC_PORT_0] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_1] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_2] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
			},
		},
		.r_arbiter[E_FJ_BUS_ARB_MXIC_R_ARBITER_R4] = {	// Image-SRAM Controller Read channel
			.port[E_FJ_BUS_ARB_MXIC_PORT_0] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_1] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
			},
			.port[E_FJ_BUS_ARB_MXIC_PORT_2] = {
				.slot0 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot1 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot2 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
				.slot3 = {
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
					D_FJ_BUS_ARB_MXIC_R_RESERVE,
				},
			},
		},
	};
	static const T_FJ_BUS_ARB_MXIC_ALL_SLOT_PRIORITY_LEVEL slot_prio = {	// Tentative
		.w_level[E_FJ_BUS_ARB_MXIC_W_ARBITER_W1] = {
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 5,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 1,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 7,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 4,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 3,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 8,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 5,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 0,
		},
		.w_level[E_FJ_BUS_ARB_MXIC_W_ARBITER_W2] = {
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 5,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 1,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 7,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 4,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 3,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 8,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 5,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 0,
		},
		.w_level[E_FJ_BUS_ARB_MXIC_W_ARBITER_W3] = {
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 5,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 1,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 7,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 4,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 3,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 8,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 5,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 0,
		},
		.w_level[E_FJ_BUS_ARB_MXIC_W_ARBITER_W4] = {
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 7,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 17,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 0,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 0,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 0,


			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 7,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 10,

			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 0,

			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 0,


			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 4,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 6,

			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 0,

			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 0,
		},
		.r_level[E_FJ_BUS_ARB_MXIC_R_ARBITER_R1] = {
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 5,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 2,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 7,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 4,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 3,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 8,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 5,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 0,
		},
		.r_level[E_FJ_BUS_ARB_MXIC_R_ARBITER_R2] = {
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 5,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 2,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 7,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 4,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 3,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 8,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 5,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 0,
		},
		.r_level[E_FJ_BUS_ARB_MXIC_R_ARBITER_R3] = {
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 5,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 2,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 7,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 4,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 3,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 8,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 5,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 3,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 0,
		},
		.r_level[E_FJ_BUS_ARB_MXIC_R_ARBITER_R4] = {
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER_ONLY,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 6,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 19,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 1,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 0,

			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 0,
			.level0[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 0,

			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 7,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 12,

			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 0,

			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 0,
			.level1[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 0,

			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].upper_priority_level = 6,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_0].lower_priority_level = 6,

			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].upper_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_1].lower_priority_level = 0,

			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].priority_level_type  = D_FJ_BUS_ARB_MXIC_SLOT_PRIORITY_UPPER,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].upper_priority_level = 0,
			.level2[E_FJ_BUS_ARB_MXIC_PORT_2].lower_priority_level = 0,
		},
	};

	E_FJ_SUB_ARB_SDRAMC_CH							sdramc_ch;
	static const T_FJ_SUB_ARB_SDRAMC_CMD_ARB_RATE	sdramc_arb_rate = {	// Tentative
		.throughput = {
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX10] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX11] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX12] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX13] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX20] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX21] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX22] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX23] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX30] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX31] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX32] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX33] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX40] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX50] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX51] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX00] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX01] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX60] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB0]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB1]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB2]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_DS0]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_DS1]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_SDM]  = 0,
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE0]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE1]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE2]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD0]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD1]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD2]  = 0,
// ---FJ_SUB_ARB_SDRAMC_PORT_ REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCOREW]	= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCORER0]	= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCORER1]	= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD0]		= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD1]		= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD2]		= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCPU]		= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCOREM]	= 0,
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		},
		.latency = {
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX10] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX11] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX12] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX13] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX20] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX21] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX22] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX23] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX30] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX31] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX32] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX33] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX40] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX50] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX51] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX00] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX01] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX60] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB0]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB1]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB2]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_DS0]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_DS1]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_SDM]  = 0,
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE0]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE1]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE2]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD0]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD1]  = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD2]  = 0,
// ---FJ_SUB_ARB_SDRAMC_PORT_ REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCOREW]	= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCORER0]	= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCORER1]	= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD0]		= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD1]		= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD2]		= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCPU]		= 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCOREM]	= 0,
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		},
		.enable = {
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX10] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX11] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX12] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX13] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX20] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX21] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX22] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX23] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX30] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX31] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX32] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX33] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX40] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX50] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX51] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX00] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX01] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX60] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB0]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB1]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB2]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_DS0]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_DS1]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_SDM]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE0]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE1]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE2]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD0]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD1]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD2]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
// ---FJ_SUB_ARB_SDRAMC_PORT_ REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCOREW]	= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCORER0]	= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCORER1]	= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD0]		= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD1]		= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD2]		= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCPU]		= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCOREM]	= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		},
	};
	static const T_FJ_SUB_ARB_SDRAMC_CMD_ARB_PRI sdramc_arb_pri = {	// Tentative
		.priority = {
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX10] = 0,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX11] = 1,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX12] = 2,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX13] = 3,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX20] = 4,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX21] = 5,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX22] = 6,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX23] = 7,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX30] = 8,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX31] = 9,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX32] = 10,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX33] = 11,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX40] = 12,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX50] = 13,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX51] = 14,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX00] = 15,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX01] = 16,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX60] = 17,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB0]  = 18,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB1]  = 19,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB2]  = 20,
			[E_FJ_SUB_ARB_SDRAMC_PORT_DS0]  = 21,
			[E_FJ_SUB_ARB_SDRAMC_PORT_DS1]  = 22,
			[E_FJ_SUB_ARB_SDRAMC_PORT_SDM]  = 23,
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE0]  = 24,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE1]  = 25,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE2]  = 26,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD0]  = 27,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD1]  = 28,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD2]  = 29,
// ---FJ_SUB_ARB_SDRAMC_PORT_ REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCOREW]	= 24,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCORER0]	= 25,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCORER1]	= 26,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD0]		= 27,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD1]		= 28,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD2]		= 29,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCPU]		= 30,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCOREM]	= 31,
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		},
		.enable = {
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX10] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX11] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX12] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX13] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX20] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX21] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX22] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX23] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX30] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX31] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX32] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX33] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX40] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX50] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX51] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX00] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX01] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_MX60] = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB0]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB1]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_RB2]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_DS0]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_DS1]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_SDM]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#ifdef CO_ES1_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES1_HARDWARE BEGIN ---
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE0]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE1]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VE2]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD0]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD1]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD2]  = E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
// ---FJ_SUB_ARB_SDRAMC_PORT_ REMOVE_ES1_HARDWARE END ---
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif // CO_ES1_HARDWARE
#ifdef CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
// --- REMOVE_ES3_HARDWARE BEGIN ---
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCOREW]	= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCORER0]	= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCORER1]	= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD0]		= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD1]		= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VD2]		= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCPU]		= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
			[E_FJ_SUB_ARB_SDRAMC_PORT_VCOREM]	= E_FJ_SUB_ARB_SDRAMC_EN_DISABLE,
// --- REMOVE_ES3_HARDWARE END --
// --- REMOVE_ES_COMPILE_OPT BEGIN ---
#endif	// CO_ES3_HARDWARE
// --- REMOVE_ES_COMPILE_OPT END ---
		},
	};

	fj_ercd = fj_busarb_set_mxicoutputportall( &output_port );
	if( fj_ercd != FJ_ERR_OK ) {
		return fj_ercd;
	}

	for( unit = E_FJ_BUS_ARB_MXIC_UNIT_1; unit < E_FJ_BUS_ARB_MXIC_UNIT_4; unit++ ) {
		fj_ercd = fj_busarb_set_mxicmasterallarbiter( unit, &arbiter_assign );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}
	}

	for( unit = E_FJ_BUS_ARB_MXIC_UNIT_0; unit < E_FJ_BUS_ARB_MXIC_UNIT_MAX; unit++ ) {
		fj_ercd = fj_busarb_set_mxicslotpriorityallarbiter( unit, &slot_prio );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}
	}

	for( sdramc_ch = E_FJ_SUB_ARB_SDRAMC_CH_0; sdramc_ch < E_FJ_SUB_ARB_SDRAMC_CH_MAX; sdramc_ch++ ) {
		fj_ercd = fj_busarb_set_sdramc_cmd_arbitration_rate( sdramc_ch, &sdramc_arb_rate );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}

		fj_ercd = fj_busarb_set_sdramc_cmd_arbitration_priority( sdramc_ch, &sdramc_arb_pri );
		if( fj_ercd != FJ_ERR_OK ) {
			return fj_ercd;
		}
	}

	return FJ_ERR_OK;
}
