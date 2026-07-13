/**
 * @file		play_index.c
 * @brief		playback thumbnail index function
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_LAST_INIT"
//#pragma arm section rodata="BASEFW_LAST_CONST"
//#pragma arm section code="BASEFW_LAST_CODE"

#include "play_attr.h"
#include "play_index.h"
#include "playback.h"
#include "fs_controller.h"	/* for UTxxx Message */
#include "h264_user.h"
#include "fj_filesystem.h"
#include "fj_host.h"
#include "osd_attr.h"
#include "sdram_map_cache_define.h"

#include "fj_demux.h"
#include "fj_filesystem.h"
#include "fj_memory.h"
#include "fj_mux_demux_common.h"

/*----------------------------------------------------------------------*/
/* Definition                                                           */
/*----------------------------------------------------------------------*/
//#define CO_PLAY_INDEX_CPU_ENDIAN_BIG

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
static T_BF_PLAY_INDEX_THUMB_ADDR	g_bf_play_index_addr_lcd[D_BF_PLAY_INDEX_NUMBER_OF_THUMB];
static T_BF_PLAY_INDEX_THUMB_ADDR	g_bf_play_index_addr_hd[D_BF_PLAY_INDEX_NUMBER_OF_THUMB];
static T_BF_PLAY_INDEX_POS			g_bf_play_index_pos_lcd[D_BF_PLAY_INDEX_NUMBER_OF_THUMB];
static T_BF_PLAY_INDEX_POS			g_bf_play_index_pos_hd[D_BF_PLAY_INDEX_NUMBER_OF_THUMB];
static T_BF_PLAY_INDEX_LIST			g_bf_play_index_list = {0};
static SHORT						g_bf_play_index_error_list[D_BF_PLAY_INDEX_NUMBER_OF_THUMB];
// --- REMOVE_DCF_EXIF BEGIN ---
static T_BF_DCF_IF_FILE_INFO		g_bf_play_index_file_info[D_BF_PLAY_INDEX_NUMBER_OF_THUMB];
// --- REMOVE_DCF_EXIF END ---

THUMBNAIL_CONFIGURATION		gBF_Play_Index_Thumbnail_Config = {
								UINT3_3,	// bThumbPerLine:	number of thumbnails per line
								UINT3_3,	// bThumbPerColumn:	number of thumbnails per column
								
								40,			// bFirstHGap:		horizontal gap size to the first thumbnail for LCD/TV
								40,			// bHGap:			horizontal gap size between thumbnail for LCD/TV
								0,			// bLastHGap:		horizontal gap size after the last thumbnail (not use)
								40,			// bFirstVGap:		vertical gap size to the first thumbnail for LCD/TV
								20,			// bVGap:			vertical gap between thumbnails for LCD/TV
								0,			// bLastVGap:		vertical gap size after the last thumbnail for LCD/TV (not use)
								
								330,		// bFirstHGap_HD:	horizontal gap size to the first thumbnail for HD
								90,			// bHGap_HD:		horizontal gap size between thumbnail for HD
								0,			// bLastHGap_HD:	horizontal gap size after the last thumbnail for HD (not use)
								90,			// bFirstVGap_HD:	vertical gap size to the first thumbnail for HD
								45,			// bVGap_HD:		vertical gap between thumbnails for HD
								0			// bLastVGap_HD:	vertical gap size after the last thumbnail for HD (not use)
							};



/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
/**
 * @brief		Set Index Current for Thumbnail
 * @param		None
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
static FJ_ERR_CODE play_index_set_current( ULONG index_list[D_BF_PLAY_INDEX_NUMBER_OF_THUMB] )
{
// --- REMOVE_DCF_EXIF BEGIN ---
	INT32	total_index;
	INT32	page_index;
	INT32	image_index;
	E_BF_MEDIA_STATE_MEDIA_TYPE tgt_media;

	INT32 valid_index = 0;
	USHORT first_index;
	ULONG current_dir = (BF_Play_Attr_Get_Image_Index()) & 0xFFFF0000;
	T_BF_DCF_IF_FILE_INFO index_file_info;
	E_BF_DCF_IF_FILE_TYPE file_type;
	UINT32 i;

	tgt_media = BF_Fs_If_Get_Cur_Media();

	switch (tgt_media) {
		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD:
			total_index = BF_Dcf_If_Get_Entry( E_BF_MEDIA_STATE_MEDIA_TYPE_SD ) & 0x0000FFFF;
			break;

		case E_BF_MEDIA_STATE_MEDIA_TYPE_SD1:
			total_index = BF_Dcf_If_Get_Entry( E_BF_MEDIA_STATE_MEDIA_TYPE_SD1 ) & 0x0000FFFF;
			break;

		case E_BF_MEDIA_STATE_MEDIA_TYPE_NF:
			total_index = BF_Dcf_If_Get_Entry( E_BF_MEDIA_STATE_MEDIA_TYPE_NF ) & 0x0000FFFF;
			break;

		default:
			BF_Debug_Print_Error(("media type error!\n"));
			return FJ_ERR_NG;
	}

	image_index = BF_Play_Attr_Get_Image_Index() & 0x0000FFFF;
	first_index = image_index;

	page_index = gBF_Play_Index_Thumbnail_Config.bThumbPerColumn * gBF_Play_Index_Thumbnail_Config.bThumbPerLine;

	for( i = 0; i <= ( total_index - image_index ); i++ ) {
		if( D_BF_FS_USER_CUSTOM_OK != BF_Dcf_If_Get_File_Info( current_dir + first_index, &index_file_info ) ) {
			break;
		}

		file_type = BF_Dcf_If_Convert_File_Type( index_file_info.file_type );

		if( file_type == E_BF_DCF_IF_FILE_TYPE_JPG ) {
			if( TRUE == BF_Play_Macro_Wrap_Check_Jpeg_Thumbnail( current_dir + first_index ) ) {
				if( valid_index < page_index ) {
					index_list[valid_index ] = current_dir + first_index;
				}
				else {
					break;
				}

				valid_index += 1;
			}
		}
		else if( file_type == E_BF_DCF_IF_FILE_TYPE_MP4 || file_type == E_BF_DCF_IF_FILE_TYPE_MOV ) {
			if( valid_index < page_index ) {
				index_list[valid_index] = current_dir + first_index;
			}
			else {
				break;
			}

			valid_index += 1;
		}

		first_index += 1;
	}

	if (valid_index == 0) {
		BF_Debug_Print_Error(("total index error!\n"));
		memset( &g_bf_play_index_list, 0x00, sizeof(T_BF_PLAY_INDEX_LIST) );
		return FJ_ERR_NG;
	}

	if (valid_index > ( total_index - image_index + 1 )) {
		BF_Debug_Print_Error(("current index error!  currentindex(%d) totalindex(%d)\n", valid_index, ( total_index - image_index )));
		return FJ_ERR_NG;
	}

	if (valid_index > page_index) {
		valid_index = page_index;
	}

	g_bf_play_index_list.total_current = valid_index;
	g_bf_play_index_list.total_page    = ((g_bf_play_index_list.total_current - 1) / page_index) + 1;
	g_bf_play_index_list.current_page  = ((image_index - 1) / page_index) + 1;
	g_bf_play_index_list.first_index   = (g_bf_play_index_list.current_page * page_index) - (page_index - 1);
	g_bf_play_index_list.thumb_count   = valid_index;

	if (g_bf_play_index_list.current_page < g_bf_play_index_list.total_page) {
		g_bf_play_index_list.thumb_count = page_index;
	}
	else if (g_bf_play_index_list.current_page == g_bf_play_index_list.total_page) {
		g_bf_play_index_list.thumb_count = (g_bf_play_index_list.total_current - g_bf_play_index_list.first_index) + 1;
	}
// --- REMOVE_DCF_EXIF END ---
	return FJ_ERR_OK;
}

/**
 * @brief		Set Index Position for Thumbnail
 * @param		i:E_BF_PLAY_COMMON_DISP_DEVICE disp_device
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
static FJ_ERR_CODE play_index_set_pos( E_BF_PLAY_COMMON_DISP_DEVICE disp_device )
{
	USHORT							index = 0;

	INT32							count_line;
	INT32							count_col;

	USHORT							draw_width;
	USHORT							draw_lines;

	UINT3							thumb_per_line;
	UINT3							thumb_per_column;
	USHORT							first_h_gap;
	USHORT							h_gap;
	USHORT							first_v_gap;
	USHORT							v_gap;
	USHORT							index_area_width;
	INT32							pos_y_tmp;

	thumb_per_line		= gBF_Play_Index_Thumbnail_Config.bThumbPerLine;
	thumb_per_column	= gBF_Play_Index_Thumbnail_Config.bThumbPerColumn;

	if (disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_HDMI) {
		draw_width = BF_Play_Attr_Get_Thumb_Draw_Width_Hdmi();
		draw_lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Hdmi();

		first_h_gap	= gBF_Play_Index_Thumbnail_Config.bFirstHGap_HD;
		h_gap		= gBF_Play_Index_Thumbnail_Config.bHGap_HD;
		first_v_gap	= gBF_Play_Index_Thumbnail_Config.bFirstVGap_HD;
		v_gap		= gBF_Play_Index_Thumbnail_Config.bVGap_HD;

		index_area_width = BF_Play_Attr_Get_Thumb_Width_Hdmi();
	}
	else {	// E_BF_PLAY_COMMON_DISP_DEVICE_LCD
		draw_width  = BF_Play_Attr_Get_Thumb_Draw_Width_Lcd();
		draw_lines  = BF_Play_Attr_Get_Thumb_Draw_Lines_Lcd();

		first_h_gap	= gBF_Play_Index_Thumbnail_Config.bFirstHGap;
		h_gap		= gBF_Play_Index_Thumbnail_Config.bHGap;
		first_v_gap	= gBF_Play_Index_Thumbnail_Config.bFirstVGap;
		v_gap		= gBF_Play_Index_Thumbnail_Config.bVGap;

		index_area_width = D_BF_PLAY_INDEX_VDE_WIDTH;
	}

	for (count_col = 0; count_col < thumb_per_column; count_col++) {
		for (count_line = 0; count_line < thumb_per_line; count_line++) {
			index = (count_col * thumb_per_line) + count_line;
			if (count_line == 0) {
				pos_y_tmp = first_v_gap + ((draw_lines + v_gap) * count_col);
				
				if (disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_HDMI) {
					g_bf_play_index_pos_hd[index].x_pos = first_h_gap;
					g_bf_play_index_pos_hd[index].y_pos = pos_y_tmp;

					g_bf_play_index_addr_hd[index].y_addr = BF_Play_Common_RoundDown_Bit_2( (index_area_width * pos_y_tmp) + first_h_gap );
					g_bf_play_index_addr_hd[index].c_addr = BF_Play_Common_RoundDown_Bit_2( ((index_area_width / 2) * (pos_y_tmp / 2) + (first_h_gap / 2)) * 2 );
				}
				else {	// E_BF_PLAY_COMMON_DISP_DEVICE_LCD
					g_bf_play_index_pos_lcd[index].x_pos = first_h_gap;
					g_bf_play_index_pos_lcd[index].y_pos = pos_y_tmp;

					g_bf_play_index_addr_lcd[index].y_addr = BF_Play_Common_RoundDown_Bit_2( (index_area_width * pos_y_tmp) + first_h_gap );
					g_bf_play_index_addr_lcd[index].c_addr = BF_Play_Common_RoundDown_Bit_2( ((index_area_width / 2) * (pos_y_tmp / 2) + (first_h_gap / 2)) * 2 );
				}
			}
			else {
				if (disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_HDMI) {
					g_bf_play_index_pos_hd[index].x_pos = g_bf_play_index_pos_hd[index - 1].x_pos + draw_width + h_gap;
					g_bf_play_index_pos_hd[index].y_pos = g_bf_play_index_pos_hd[index - 1].y_pos;

					g_bf_play_index_addr_hd[index].y_addr  = BF_Play_Common_RoundDown_Bit_2( g_bf_play_index_addr_hd[index - 1].y_addr + draw_width + h_gap );
					g_bf_play_index_addr_hd[index].c_addr  = BF_Play_Common_RoundDown_Bit_2( g_bf_play_index_addr_hd[index - 1].c_addr + draw_width + h_gap );
				}
				else {	// E_BF_PLAY_COMMON_DISP_DEVICE_LCD
					g_bf_play_index_pos_lcd[index].x_pos = g_bf_play_index_pos_lcd[index - 1].x_pos + draw_width + h_gap;
					g_bf_play_index_pos_lcd[index].y_pos = g_bf_play_index_pos_lcd[index - 1].y_pos;

					g_bf_play_index_addr_lcd[index].y_addr  = BF_Play_Common_RoundDown_Bit_2( g_bf_play_index_addr_lcd[index - 1].y_addr + draw_width + h_gap );
					g_bf_play_index_addr_lcd[index].c_addr  = BF_Play_Common_RoundDown_Bit_2( g_bf_play_index_addr_lcd[index - 1].c_addr + draw_width + h_gap );
				}
			}
		}
	}
	
	return FJ_ERR_OK;
}

/**
 * @brief		Make Black Image of YCC420
 * @param		i:T_BF_PLAY_MACRO_WRAP_ADDR dst_addrs
 * @param		i:USHORT width
 * @param		i:USHORT lines
 * @param		i:T_BF_PLAY_MACRO_WRAP_COLOR color_param
 * @param		i:T_BF_PLAY_MACRO_WRAP_FILL_POS bg_pos
 * @return		None
 * @attentio	None
 */
VOID play_index_black_fill_ycc420( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* dst_addr_size, USHORT width, USHORT lines, T_BF_PLAY_MACRO_WRAP_COLOR color_param, T_BF_PLAY_MACRO_WRAP_FILL_POS bg_pos )
{
	T_BF_PLAY_MACRO_WRAP_ADDR		addrs;

	if (((bg_pos.fill_x_pos + width) > dst_addr_size->size.width) || ((bg_pos.fill_y_pos + lines) > dst_addr_size->size.lines)) {
		BF_Debug_Print_Error(("size overr error! posX, Y(%d,%d) width, lines(%d,%d)\n", bg_pos.fill_x_pos, bg_pos.fill_y_pos, width, lines));
		return;
	}

	addrs.addr_a = (dst_addr_size->addrs.addr_a + ((dst_addr_size->size.global_width *  bg_pos.fill_y_pos)      + bg_pos.fill_x_pos));
	addrs.addr_b = (dst_addr_size->addrs.addr_b + ((dst_addr_size->size.global_width * (bg_pos.fill_y_pos / 2)) + bg_pos.fill_x_pos));

	BF_Play_Macro_Wrap_Fill_Image(&(addrs), 
									dst_addr_size->size.global_width, width, lines, 
									color_param.fill_y_color, color_param.fill_cb_color, color_param.fill_cr_color, 
									E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, 
									E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR, 
									E_BF_PLAY_MACRO_WRAP_FILL_ALL,
									E_BF_PLAY_MACRO_WRAP_USED_2CH);
	return;
}

static INT32 play_index_get_demux_video_size( UINT32 width, UINT32 lines, E_FJ_MOVIE_PLAY_VIDEO_SIZE * image_size )
{
	if( ( width == 4000 ) && ( lines == 3000 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_4000_3000;
	}
	else if( ( width == 4096 ) && ( lines == 2304 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2304;
	}
	else if( ( width == 4096 ) && ( lines == 2160 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2160;
	}
	else if( ( width == 4096 ) && ( lines == 2048 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_4096_2048;
	}
	else if( ( width == 3840 ) && ( lines == 2160 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_3840_2160;
	}
	else if( ( width == 3840 ) && ( lines == 1920 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_3840_1920;
	}
	else if( ( width == 3000 ) && ( lines == 3000 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_3000_3000;
	}
	else if( ( width == 2704 ) && ( lines == 2028 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_2704_2028;
	}
	else if( ( width == 2704 ) && ( lines == 1520 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_2704_1520;
	}
	else if( ( width == 2560 ) && ( lines == 1920 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_2560_1920;
	}
	else if( ( width == 2560 ) && ( lines == 1440 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_2560_1440;
	}
	else if( ( width == 1920 ) && ( lines == 1440 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1440;
	}
	else if( ( width == 1920 ) && ( lines == 1080 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1080;
	}
	else if( ( width == 1920 ) && ( lines == 960 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_960;
	}
	else if( ( width == 1504 ) && ( lines == 1504 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1504_1504;
	}
	else if( ( width == 1440 ) && ( lines == 1080 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1440_1080;
	}
	else if( ( width == 1440 ) && ( lines == 720 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1440_720;
	}
	else if( ( width == 1280 ) && ( lines == 960 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_960;
	}
	else if( ( width == 1280 ) && ( lines == 720 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_720;
	}
	else if( ( width == 960 ) && ( lines == 540 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_960_540;
	}
	else if( ( width == 960 ) && ( lines == 480 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_960_480;
	}
	else if( ( width == 864 ) && ( lines == 480 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_864_480;
	}
	else if( ( width == 848 ) && ( lines == 480 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_848_480;
	}
	else if( ( width == 720 ) && ( lines == 480 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_720_480;
	}
	else if( ( width == 640 ) && ( lines == 480 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_480;
	}
	else if( ( width == 640 ) && ( lines == 360 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_360;
	}
	else if( ( width == 480 ) && ( lines == 270 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_480_270;
	}
	else if( ( width == 432 ) && ( lines == 240 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_432_240;
	}
	else if( ( width == 320 ) && ( lines == 240 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_320_240;
	}
	else if( ( width == 720 ) && ( lines == 576 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_720_576;
	}
	else if( ( width == 1024 ) && ( lines == 768 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1024_768;
	}
	else if( ( width == 1280 ) && ( lines == 768 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_768;
	}
	else if( ( width == 1280 ) && ( lines == 1024 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1280_1024;
	}
	else if( ( width == 1600 ) && ( lines == 896 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1600_896;
	}
	else if( ( width == 1024 ) && ( lines == 576 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1024_576;
	}
	else if( ( width == 800 ) && ( lines == 448 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_800_448;
	}
	else if( ( width == 352 ) && ( lines == 288 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_352_288;
	}
	else if( ( width == 2592 ) && ( lines == 1944 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_2592_1944;
	}
	else if( ( width == 2304 ) && ( lines == 1296 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_2304_1296;
	}
	else if( ( width == 1920 ) && ( lines == 1280 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_1920_1280;
	}
	else if( ( width == 640 ) && ( lines == 512 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_640_512;
	}
	else if( ( width == 384 ) && ( lines == 288 ) ) {
		*image_size	= E_FJ_MOVIE_PLAY_VIDEO_SIZE_384_288;
	}

	else {
		return -1;
	}

	return 0;
}

/**
 * @brief		Init Index Current for Thumbnail
 * @param		i:ULONG  work_addr
 * @param		i:E_BF_PLAY_COMMON_DISP_DEVICE disp_device
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID play_index_init( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* work_addr_size, E_BF_PLAY_COMMON_DISP_DEVICE disp_device )
{
	T_BF_PLAY_MACRO_WRAP_COLOR		thumb_color;
	T_BF_PLAY_MACRO_WRAP_FILL_POS	thumb_pos;

	memset( &thumb_pos, 0x00, sizeof(T_BF_PLAY_MACRO_WRAP_FILL_POS) );
	
	/* Fill VRAM area */
	BF_Play_Macro_Wrap_Fill_Image(&(work_addr_size->addrs), 
									work_addr_size->size.global_width, work_addr_size->size.width, work_addr_size->size.lines, 
									D_BF_PLAY_MACRO_WRAP_FILL_BLACK_Y, D_BF_PLAY_MACRO_WRAP_FILL_BLACK_CB, D_BF_PLAY_MACRO_WRAP_FILL_BLACK_CR, 
									E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, 
									E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR, 
									E_BF_PLAY_MACRO_WRAP_FILL_ALL,
									E_BF_PLAY_MACRO_WRAP_USED_2CH);
	
	thumb_color.fill_y_color  = (UCHAR)(BF_Play_Attr_Get_Thumb_Bgc_Yu() >> 8);
	thumb_color.fill_cb_color = (UCHAR)BF_Play_Attr_Get_Thumb_Bgc_Yu();
	thumb_color.fill_cr_color = thumb_color.fill_cb_color;

	if (disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_HDMI) {
		thumb_pos.fill_x_pos = BF_Play_Attr_Get_Thumb_Left_Hdmi();
		thumb_pos.fill_y_pos = BF_Play_Attr_Get_Thumb_Top_Hdmi();
		
		play_index_black_fill_ycc420( work_addr_size, BF_Play_Attr_Get_Thumb_Width_Hdmi(), BF_Play_Attr_Get_Thumb_Height_Hdmi(), thumb_color, thumb_pos );
		
	}
	else {	// E_BF_PLAY_COMMON_DISP_DEVICE_LCD
		thumb_pos.fill_x_pos = BF_Play_Attr_Get_Thumb_Left_Lcd();
		thumb_pos.fill_y_pos = BF_Play_Attr_Get_Thumb_Top_Lcd();
		
		play_index_black_fill_ycc420( work_addr_size, BF_Play_Attr_Get_Thumb_Width_Lcd(), BF_Play_Attr_Get_Thumb_Height_Lcd(), thumb_color, thumb_pos );
	}
}

// --- REMOVE_DCF_EXIF BEGIN ---
/**
 * @brief		Jpeg image display process for Thumbnail
 * @param		i:INT32 index_num
 * @param		i:USHORT disp_index
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* work_addr_size
 * @param		o:T_BF_DCF_IF_FILE_INFO* file_info
 * @param		i:E_BF_PLAY_COMMON_DISP_DEVICE disp_device
 * @return		0:normal end -1:abnormal end
 * @note		None
 * @attention	None
 */
static INT32 play_index_jpeg( INT32 index_num, ULONG disp_index, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* image_addr_size, T_BF_DCF_IF_FILE_INFO* file_info, E_BF_PLAY_COMMON_DISP_DEVICE disp_device )
{
	INT32	ret;
	ULONG	thumb_load_addr;
	ULONG	thumb_decode_addr;
	T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG	jpgmng;
	T_BF_DCF_IF_IMAGE_DETAIL			img_detail;
	T_BF_DCF_IF_LOAD_IMAGE_DATA			load_img_data;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR		src;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR		dst;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR		cpy_dst;
	E_BF_DCF_IF_ROTATE					current_rotate;
	USHORT								offset_index;
	T_IMAGE_SIZE						dst_thumb_size;
	USHORT								dst_thumb_g_width;
	T_BF_PLAY_MACRO_WRAP_ADDR 			image_addrs;
	E_BF_PLAY_MACRO_WRAP_PFMT			img_frmt;

	/* initialize structure */
	memset( &img_detail,     0x00, sizeof(T_BF_DCF_IF_IMAGE_DETAIL) );
	memset( &src,            0x00, sizeof(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR) );
	memset( &dst,            0x00, sizeof(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR) );
	memset( &jpgmng,         0x00, sizeof(T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG) );
	memset( &dst_thumb_size, 0x00, sizeof(T_IMAGE_SIZE) );

	/**************************/
	/* Jpeg Data Load Process */
	/**************************/
	/* get load area address */
	thumb_load_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Ahead_Load_Addr() );
	thumb_decode_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Image_Work_Addr() );
	ret = BF_Play_Macro_Wrap_Get_Jpeg( FJ_MODE_DISP_IMAGE_THUMB, disp_index, (UCHAR*)thumb_load_addr, (UCHAR*)thumb_decode_addr, &jpgmng, &img_detail, &load_img_data, TRUE, TRUE );
	if (ret != 0){
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Get_Jpeg ERR. err=%d, current no=%lu\n", ret, disp_index));
		return -1;
	}

	/* file info copy */
	memcpy( file_info, &img_detail.file_info, sizeof(T_BF_DCF_IF_FILE_INFO) );
	file_info->jpeg_format = img_detail.jpeg_format;

	if (disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_HDMI) {
		dst_thumb_size.width = BF_Play_Attr_Get_Thumb_Draw_Width_Hdmi();
		dst_thumb_size.lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Hdmi();
	}
	else {	// E_BF_PLAY_COMMON_DISP_DEVICE_LCD
		dst_thumb_size.width = BF_Play_Attr_Get_Thumb_Draw_Width_Lcd();
		dst_thumb_size.lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Lcd();
	}
	dst_thumb_g_width = dst_thumb_size.width;
	if (dst_thumb_g_width % 32) {
		dst_thumb_g_width = ((dst_thumb_g_width >> 5) << 5) + 32;
	}

	/******************************/
	/* Convert YCbCr Data Process */
	/******************************/
	/* set source parameter */
	BF_Play_Macro_Wrap_Set_Size_Addr( &src, &jpgmng, jpgmng.width, jpgmng.lines, jpgmng.gl_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_JPEGMNG );

	/* set destination parameter */
	BF_Play_Macro_Wrap_Set_Size_Addr( &dst, &thumb_load_addr, dst_thumb_size.width, dst_thumb_size.lines, dst_thumb_g_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );

	/* check lotation */
	BF_Play_Common_Rotate_Check_Exif_Tag( (E_BF_DCF_IF_ROTATE)img_detail.rotate, &current_rotate );

	switch (jpgmng.ycbcr_smpl) {
		case E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC422:
			img_frmt = E_BF_PLAY_MACRO_WRAP_PFMT_YCC422;
			break;
		case E_BF_PLAY_MACRO_WRAP_JPG_SMPL_YCC420:
			img_frmt = E_BF_PLAY_MACRO_WRAP_PFMT_YCC420;
			break;
		default:
			return -1;
	}
	ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( src, img_frmt, &dst, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420,
										   current_rotate, E_BF_PLAY_MACRO_WRAP_ADJUST_ON );
	if (ret != 0) {
		return -1;
	}

	offset_index = index_num % gBF_Play_Index_Thumbnail_Config.bThumbPerLine;
	if (offset_index >= D_BF_PLAY_INDEX_NUM_LINE_THUMB) {
		return -1;
	}
	
	/**********************************/
	/* Set Thumbnail into VGA Process */
	/**********************************/

	if (disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_HDMI) {
		image_addrs.addr_a = image_addr_size->addrs.addr_a + g_bf_play_index_addr_hd[index_num].y_addr;
		image_addrs.addr_b = image_addr_size->addrs.addr_b + g_bf_play_index_addr_hd[index_num].c_addr;
		BF_Play_Macro_Wrap_Set_Size_Addr( &cpy_dst, &image_addrs, dst_thumb_size.width, dst_thumb_size.lines, image_addr_size->size.global_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR );
	}
	else {	// E_BF_PLAY_COMMON_DISP_DEVICE_LCD
		image_addrs.addr_a = image_addr_size->addrs.addr_a + g_bf_play_index_addr_lcd[index_num].y_addr;
		image_addrs.addr_b = image_addr_size->addrs.addr_b + g_bf_play_index_addr_lcd[index_num].c_addr;
		BF_Play_Macro_Wrap_Set_Size_Addr( &cpy_dst, &image_addrs, dst_thumb_size.width, dst_thumb_size.lines, image_addr_size->size.global_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR );
	}
	BF_Play_Macro_Wrap_Make_Image_With_Back(dst, cpy_dst, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420);
	return 0;
}


/**
 * @brief		Wave dummy image display process for Thumbnail
 * @param		i:INT32 index_num
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* work_addr_size
 * @param		i:E_BF_PLAY_COMMON_DISP_DEVICE disp_device
 * @return		0:normal end -1:abnormal end
 * @note		None
 * @attention	None
 */
static INT32 play_index_wave( INT32 index_num, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* work_addr_size, E_BF_PLAY_COMMON_DISP_DEVICE disp_device )
{
#if 0
	INT32							ret;
	ULONG							load_addr;
	UCHAR							offset_y;
	UCHAR							offset_cbcr;
	T_IM_JPEG_DEC_MNG				jpgmng;
	T_BF_PLAY_INDEX_PARAM			thumb_param;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	src;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	dst;
	USHORT							offset_index;
	USHORT							draw_width;
	USHORT							draw_lines;

	if (disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_HDMI) {
		draw_width = BF_Play_Attr_Get_Thumb_Draw_Width_Hd();
		draw_lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Hd();
	}
	else {	// E_BF_PLAY_COMMON_DISP_DEVICE_LCD
		draw_width = BF_Play_Attr_Get_Thumb_Draw_Width_Vld();
		draw_lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Vld();
	}

	/* initialize structure */
	memset( &jpgmng,      0x00, sizeof(T_IM_JPEG_DEC_MNG) );
	memset( &src,         0x00, sizeof(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR) );
	memset( &dst,         0x00, sizeof(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR) );
	memset( &thumb_param, 0x00, sizeof(T_BF_PLAY_INDEX_PARAM) );

	load_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Index_Load_Addr() );

	BF_Play_Common_Load_Jpeg( FJ_CUSTOM_PB_THUMB_AUDIO, load_addr, &jpgmng, PIX_SIZ_THUMB );

	/****************************/
	/* Jpeg Data Decode Process */
	/****************************/
	jpgmng.code_addr     = load_addr;
	jpgmng.ycbcr_addr[0] = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)SDRAM_ADR_PLAY_THUMBINDEX_REC );
	jpgmng.ycbcr_addr[1] = jpgmng.ycbcr_addr[0] +  PIX_SIZ_THUMB;
	jpgmng.ycbcr_addr[2] = jpgmng.ycbcr_addr[1] + (PIX_SIZ_THUMB / 2);
	/* Decode Type Set */
	jpgmng.ycbcr_smpl    = E_IM_JPEG_YCC422;

//	/* SDRAM buffer flash */
//	IO_JSD.FRICTL1.hword |= 0x00C0 ;

	/* decompression JPEG data to YCbCr */
	if (BF_Play_Macro_Wrap_Decode_Jpeg( &jpgmng ) != 0) {
		return -1;
	}
	/******************************/
	/* Convert YCbCr Data Process */
	/******************************/
	/* set source parameter */
	BF_Play_Macro_Wrap_Set_Size_Addr( &src, &jpgmng, jpgmng.width, jpgmng.lines, jpgmng.gl_width, E_BF_PLAY_MACRO_WRAP_PFMT_NONE, E_BF_PLAY_MACRO_WRAP_SET_FORM_JPEGMNG );

	/* set destination parameter */
	BF_Play_Macro_Wrap_Set_Size_Addr( &dst, &load_addr, draw_width, draw_lines, draw_width, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );

	/* convert from YCbCr420 to YCbCr420 */
	if (jpgmng.ycbcr_smpl == E_IM_JPEG_YCC420) {
		/* convert data from YCbCr420 to YCbCr420 */
		ret = BF_Play_Macro_Wrap_Convert_Size( src, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, dst, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF, E_IM_REC_CONV_MD_BI_LINEAR );
	}
	else if (jpgmng.ycbcr_smpl == E_IM_JPEG_YCC444) {
		/* convert data from YCbCr422 to YCbCr420 */
		ret = BF_Play_Macro_Wrap_Convert_Size( src, E_BF_PLAY_MACRO_WRAP_PFMT_YCC444, dst, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF, E_IM_REC_CONV_MD_BI_LINEAR );
	}
	else {
		/* convert data from YCbCr422 to YCbCr420 */
		ret = BF_Play_Macro_Wrap_Convert_Size( src, E_BF_PLAY_MACRO_WRAP_PFMT_YCC422, dst, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF, E_IM_REC_CONV_MD_BI_LINEAR );
	}
	if (ret != 0) {
		return -1;
	}

	thumb_param.src.y_addr  = dst.addrs.addr_a;
	thumb_param.src.cb_addr = dst.addrs.addr_b;
	thumb_param.src.cr_addr = dst.addrs.addr_c;
	if (disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_HDMI) {
		thumb_param.dst.y_addr  = (*work_addr + g_bf_play_index_addr_hd[index_num].y_addr);
		thumb_param.dst.cb_addr = (*work_addr + PIX_SIZ_3M_16_9 + g_bf_play_index_addr_hd[index_num].cb_addr);
		thumb_param.dst.cr_addr = (*work_addr + PIX_SIZ_3M_16_9 + (PIX_SIZ_3M_16_9 / 4) + g_bf_play_index_addr_hd[index_num].cr_addr);
	}
	else {	// E_BF_PLAY_COMMON_DISP_DEVICE_LCD
		thumb_param.dst.y_addr  = (*work_addr + g_bf_play_index_addr_vld[index_num].y_addr);
		thumb_param.dst.cb_addr = (*work_addr + PIX_SIZ_VGA + g_bf_play_index_addr_vld[index_num].cb_addr);
		thumb_param.dst.cr_addr = (*work_addr + PIX_SIZ_VGA + PIX_SIZ_QVGA + g_bf_play_index_addr_vld[index_num].cr_addr);
	}

	offset_index = index_num % gBF_Play_Index_Thumbnail_Config.bThumbPerLine;
	if (offset_index >= D_BF_PLAY_INDEX_NUM_LINE_THUMB) {
		return -1;
	}

	if (disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_HDMI) {
		offset_y    = g_bf_play_index_offset_hd.x_offset_y[offset_index];
		offset_cbcr = g_bf_play_index_offset_hd.x_offset_c[offset_index];
	}
	else {	// E_BF_PLAY_COMMON_DISP_DEVICE_LCD
		offset_y    = g_bf_play_index_offset_vld.x_offset_y[offset_index];
		offset_cbcr = g_bf_play_index_offset_vld.x_offset_c[offset_index];
	}

	play_index_thinout( thumb_param, offset_y, offset_cbcr, disp_device );
#endif
	return 0;
}

/**
 * @brief		Illigal image display process for Thumbnail
 * @param		i:INT32 index_num
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* work_addr_size
 * @return		0:normal end -1:abnormal end
 * @note		None
 * @attention	None
 */
static INT32 play_index_illigal( INT32 index_num, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* work_addr_size )
{
#if 0
	ULONG							load_addr;
	//ULONG							dst_addrs;
	UCHAR							offset_y;
	UCHAR							offset_cbcr;
	T_IM_JPEG_DEC_MNG				jpgmng;
	T_BF_PLAY_INDEX_PARAM			thumb_param;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	src;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	dst;
	USHORT							offset_index;
	USHORT							draw_width;
	USHORT							draw_lines;

	draw_width = BF_Play_Attr_Get_Thumb_Draw_Width_Vld();
	draw_lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Vld();

	/* initialize structure */
	memset( &jpgmng, 0x00, sizeof(T_IM_JPEG_DEC_MNG) );
	memset( &src, 0x00, sizeof(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR) );
	memset( &dst, 0x00, sizeof(T_BF_PLAY_MACRO_WRAP_SIZE_ADDR) );
	memset( &thumb_param, 0x00, sizeof(T_BF_PLAY_INDEX_PARAM) );

	/****************************/
	/* Load QVGA Data Process   */
	/****************************/
	/* get load area address */
	load_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Index_Load_Addr() );

	BF_Play_Common_Load_Jpeg( FJ_CUSTOM_PB_THUMB_ERR, load_addr, &jpgmng, PIX_SIZ_THUMB );

	/****************************/
	/* Jpeg Data Decode Process */
	/****************************/
	jpgmng.code_addr     = load_addr;
	jpgmng.ycbcr_addr[0] = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)SDRAM_ADR_PLAY_THUMBINDEX_REC );
	jpgmng.ycbcr_addr[1] = (jpgmng.ycbcr_addr[0] + SDRAM_SIZ_PLAY_THUMB_WORK);
	jpgmng.ycbcr_addr[2] = (jpgmng.ycbcr_addr[1] + (PIX_SIZ_THUMB / 2));
	jpgmng.ycbcr_smpl    = E_IM_JPEG_YCC422;	// Decode Type Set

	/* decompression JPEG data to YCbCr */
	if (BF_Play_Macro_Wrap_Decode_Jpeg( &jpgmng ) != 0) {
		return -1;
	}
	/******************************/
	/* Convert YCbCr Data Process */
	/******************************/
	/* set source parameter */
	BF_Play_Macro_Wrap_Set_Size_Addr( &src, &jpgmng, jpgmng.width, jpgmng.lines, jpgmng.gl_width, E_BF_PLAY_MACRO_WRAP_PFMT_NONE, E_BF_PLAY_MACRO_WRAP_SET_FORM_JPEGMNG );

	/* set destination parameter */
	//dst_addrs = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Image_Work_Addr() );
	BF_Play_Macro_Wrap_Set_Size_Addr( &dst, &load_addr, draw_width, draw_lines, draw_width, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );

	/* convert from YCbCr420 to YCbCr420 */
	if (BF_Play_Macro_Wrap_Convert_Size( src, E_BF_PLAY_MACRO_WRAP_PFMT_YCC422, dst, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF, E_IM_REC_CONV_MD_BI_LINEAR ) != 0) {
		return -1;
	}

	thumb_param.src.y_addr  = dst.addrs.addr_a;
	thumb_param.src.cb_addr = dst.addrs.addr_b;
	thumb_param.src.cr_addr = dst.addrs.addr_c;
	if (disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_HDMI) {
		thumb_param.dst.y_addr  = (*work_addr + g_bf_play_index_addr_hd[index_num].y_addr);
		thumb_param.dst.cb_addr = (*work_addr + PIX_SIZ_VGA + g_bf_play_index_addr_hd[index_num].cb_addr);
		thumb_param.dst.cr_addr = (*work_addr + PIX_SIZ_VGA + PIX_SIZ_QVGA + g_bf_play_index_addr_hd[index_num].cr_addr);
	}
	else {	// E_BF_PLAY_COMMON_DISP_DEVICE_LCD
		thumb_param.dst.y_addr  = (*work_addr + g_bf_play_index_addr_vld[index_num].y_addr);
		thumb_param.dst.cb_addr = (*work_addr + PIX_SIZ_VGA + g_bf_play_index_addr_vld[index_num].cb_addr);
		thumb_param.dst.cr_addr = (*work_addr + PIX_SIZ_VGA + PIX_SIZ_QVGA + g_bf_play_index_addr_vld[index_num].cr_addr);
	}

	offset_index = index_num % gBF_Play_Index_Thumbnail_Config.bThumbPerLine;
	if (offset_index >= D_BF_PLAY_INDEX_NUM_LINE_THUMB) {
		return -1;
	}

	if (disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_HDMI) {
		offset_y    = g_bf_play_index_offset_hd.x_offset_y[offset_index];
		offset_cbcr = g_bf_play_index_offset_hd.x_offset_c[offset_index];
	}
	else {	// E_BF_PLAY_COMMON_DISP_DEVICE_LCD
		offset_y    = g_bf_play_index_offset_vld.x_offset_y[offset_index];
		offset_cbcr = g_bf_play_index_offset_vld.x_offset_c[offset_index];
	}

	play_index_thinout( thumb_param, offset_y, offset_cbcr, disp_device );
#endif
	return 0;
}

/**
 * @brief		Movie image display process for Thumbnail
 * @param		i:INT32 index_num
 * @param		i:USHORT disp_index
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* work_addr_size
 * @param		i:E_BF_PLAY_COMMON_DISP_DEVICE disp_device
 * @param		i:E_BF_DCF_IF_FILE_TYPE file_type
 * @return		0:normal end -1:abnormal end
 * @note		None
 * @attention	None
 */
static INT32 play_index_movie( INT32 index_num, ULONG disp_index, T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* work_addr_size, E_BF_PLAY_COMMON_DISP_DEVICE disp_device, E_BF_DCF_IF_FILE_TYPE file_type )
{
#if 0 // PORTING
	INT32	ret = 0;
	T_BF_H264_USER_TOP_IMAGE		h264_top_image;
	T_BF_PLAY_MACRO_WRAP_ADDR		top_image_addr;
	USHORT							top_image_width;
	USHORT							top_image_lines;
	E_BF_PLAY_MACRO_WRAP_PFMT		top_image_format;
	T_BF_PLAY_MACRO_WRAP_ADDR		top_image2_addr;
	USHORT							top_image2_width;
	USHORT							top_image2_lines;
	ULONG							addr_offset_y;
	ULONG							addr_offset_c;
	T_BF_PLAY_MACRO_WRAP_ADDR		top_copy_addr;
	USHORT							top_copy_width;
	USHORT							top_copy_lines;
	ULONG							dst_addrs;
	ULONG							mid_addrs;
	T_BF_DCF_IF_FILE_INFO			file_info;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR	src,dst,mid;
	T_BF_PLAY_MACRO_WRAP_ADDR		image_addrs;
	USHORT							offset_index;
	USHORT							draw_width;
	USHORT							draw_lines;
	USHORT							draw_g_width;
	ULONG							pix_size = 0;
	ULONG							current_index = BF_Play_Attr_Get_Image_Index();
	USHORT							current_filetype = BF_Play_Attr_Get_File_Type();
	CHAR							movie_ext[4] = "THM";
	INT32							file_id = 0;
	FW_CHAR							h264_file_path[D_BF_H264_USER_FNAME_MAX];
	ULONG							movie_work_top_img = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Image_Work_Addr() );
	ULONG							movie_work_top_img_2_disp = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)(movie_work_top_img + (PIX_SIZ_FHD * 2)) );
	ULONG							movie_work_top_img_2_34 = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)(movie_work_top_img_2_disp + (PIX_SIZ_FHD * 2)) );
	ULONG							movie_work_top_img_2_dst = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)(movie_work_top_img_2_34 + ((WIDTH_FHD * ((WIDTH_FHD * 3) / 4)) * 2)) );
	//size(movie_work_top_img) + size(movie_work_top_img_2_disp) + size(movie_work_top_img_2_34) + size(movie_work_top_img_2_dst) < 0x1123800 < SDRAM_SIZ_PLAY_IMAGE_DECODE_AREA(0x44F4800)
	
	/******************************/
	/* Get Mpeg Top Image Process */
	/******************************/
	/* get current file info */
	ret = BF_Dcf_If_Get_File_Info( disp_index, &file_info );
	if (ret != 0) {
		BF_Debug_Print_Error(("BF_Dcf_If_Get_File_Info err on play_index_movie. ret=%d\n", ret));
		return -1;
	}
	
	fj_setcurrentimage( (INT32)disp_index );
	
	top_image_addr.addr_a = movie_work_top_img;
	top_image_addr.addr_b = top_image_addr.addr_a +  PIX_SIZ_FHD;
	
	switch (file_type ) {
		case E_BF_DCF_IF_FILE_TYPE_MP4 :		// Fallthrough
		case E_BF_DCF_IF_FILE_TYPE_MOV :
			if(file_type == E_BF_DCF_IF_FILE_TYPE_MP4) {
//				fj_setfiletype( 64 );	// MP4 file
				fj_setfiletype( FJ_FILE_TYPE_MP4 );	// MP4 file
			}
			else {
//				fj_setfiletype( 1024 );	// MOV file
				fj_setfiletype( FJ_FILE_TYPE_MOV );	// MOV file
			}
			
			h264_top_image.addr_y  = top_image_addr.addr_a;
			h264_top_image.addr_c = top_image_addr.addr_b;
			
			/* Get File Path Information */
			ret = BF_Dcf_If_Get_Index_File_Path( disp_index, (const char*)&movie_ext[0], h264_file_path );
			if( ret != D_OS_USER_E_OK ){
				BF_Debug_Print_Error(("BF_Dcf_If_Get_Index_File_Path Error=%d\n", ret));
				return -1;
			}
			/* File Open */
			file_id = BF_Fs_If_Open( (const char*)h264_file_path, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_NORMAL );
			BF_Fs_If_Close( file_id );
			if (file_id == 0) {		// THM file not exist
				ret = BF_H264_User_Play_Top( D_BF_H264_USER_ON, &h264_top_image );
			}
			else {					// THM file exist
//				BF_Fs_If_Close( file_id );
				ret = BF_H264_User_Play_Get_THM_Image( &h264_top_image );
			}
			if (ret != 0) {
				BF_Debug_Print_Error(("BF_H264_User_Play_Top err on play_index_movie. ret=%d\n", ret));
				fj_setcurrentimage( (INT32)current_index );
				fj_setfiletype(current_filetype);
				return -1;
			}
			
			if (file_id == 0) {		// THM file not exist
				top_image_width					= h264_top_image.width;
				top_image_lines					= h264_top_image.lines;
			}
			else {					// THM file exist
				top_image_width					= WIDTH_THUMB;
				top_image_lines					= LINES_THUMB;
				h264_top_image.width			= WIDTH_THUMB;
				h264_top_image.lines			= LINES_THUMB;
				h264_top_image.display_width	= WIDTH_THUMB;
				h264_top_image.display_lines	= LINES_THUMB;
			}
			
			switch (h264_top_image.format) {
				case E_BF_H264_USER_IMAGE_FORMAT_420 :
					top_image_format = E_BF_PLAY_MACRO_WRAP_PFMT_YCC420;
					break;
					
				default :
					top_image_format = E_BF_PLAY_MACRO_WRAP_PFMT_YCC422;
					break;
			}
			break;
			
		default :
			fj_setcurrentimage( (INT32)current_index );
			return -1;
	}

	fj_setcurrentimage( (INT32)current_index );
	fj_setfiletype(current_filetype);

	if (file_type != E_BF_DCF_IF_FILE_TYPE_AVI && ((h264_top_image.width != h264_top_image.display_width) || (h264_top_image.lines != h264_top_image.display_lines))) {
		mid_addrs = movie_work_top_img_2_disp;

		BF_Play_Macro_Wrap_Set_Size_Addr( &src, &top_image_addr, top_image_width, top_image_lines, top_image_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR );
		BF_Play_Macro_Wrap_Set_Size_Addr( &mid, &mid_addrs, h264_top_image.display_width, h264_top_image.display_lines, h264_top_image.display_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );
		BF_Play_Macro_Wrap_Rotate_Convert_Image( src, top_image_format, &mid, top_image_format, E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF );

		memcpy( &top_image_addr, &mid.addrs, sizeof(T_BF_PLAY_MACRO_WRAP_ADDR) );
		top_image_width = h264_top_image.display_width;
		top_image_lines = h264_top_image.display_lines;
	}

	if ((top_image_width * 3) != (top_image_lines * 4)) {

		pix_size = WIDTH_FHD * ((WIDTH_FHD * 3) / 4);

		top_image2_addr.addr_a = movie_work_top_img_2_34;
		top_image2_addr.addr_b = top_image2_addr.addr_a + pix_size;

		if ((top_image_width * 3) > (top_image_lines * 4)) {
			top_image2_width =  top_image_width;
			top_image2_lines = (top_image_width * 3) / 4;
			
			addr_offset_y = top_image2_width * ((top_image2_lines - top_image_lines) / 2);
			switch (top_image_format) {
				case E_BF_PLAY_MACRO_WRAP_PFMT_YCC420 :
					addr_offset_c = top_image2_width * (((top_image2_lines / 2) - (top_image_lines / 2)) / 2);
					break;
					
				default :
					addr_offset_c = top_image2_width * ((top_image2_lines - top_image_lines) / 2);
					break;
			}
		}
		else {
			top_image2_lines =  top_image_lines;
			top_image2_width = (top_image_lines * 4) / 3;
			
			addr_offset_y = (top_image2_width - top_image_width) / 2;
			addr_offset_c = ((top_image2_width) - (top_image_width)) / 2;
		}
		top_copy_addr.addr_a = top_image2_addr.addr_a + addr_offset_y;
		top_copy_addr.addr_b = top_image2_addr.addr_b + addr_offset_c;
		top_copy_width       = top_image_width;
		top_copy_lines       = top_image_lines;
		
		BF_Play_Macro_Wrap_Fill_Image( (VOID*)(&top_image2_addr.addr_a),
										WIDTH_FHD, top_image2_width, top_image2_lines,
										D_BF_PLAY_MACRO_WRAP_FILL_BLACK_Y, D_BF_PLAY_MACRO_WRAP_FILL_BLACK_CB, D_BF_PLAY_MACRO_WRAP_FILL_BLACK_CR,
										top_image_format,
										E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG,
										E_BF_PLAY_MACRO_WRAP_FILL_ALL,
										E_BF_PLAY_MACRO_WRAP_USED_2CH );
		
		// copy (top_image -> top_copy)
		BF_Play_Macro_Wrap_Copy_Image(top_image_addr.addr_a, top_image_addr.addr_b, top_image_width, 
										top_copy_addr.addr_a, top_copy_addr.addr_b, top_image2_width, 
										top_copy_width, top_copy_lines, 											
										top_image_format, 
										E_BF_PLAY_MACRO_WRAP_USED_2CH);
		
		memcpy( &top_image_addr, &top_image2_addr, sizeof(T_BF_PLAY_MACRO_WRAP_ADDR) );
		top_image_width = top_image2_width;
		top_image_lines = top_image2_lines;
	}
	
	/******************************/
	/* Convert YCbCr Data Process */
	/******************************/
	/* set source parameter */
	BF_Play_Macro_Wrap_Set_Size_Addr( &src, &top_image_addr, top_image_width, top_image_lines, top_image_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR );
	
	if (disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_HDMI) {
		draw_width = BF_Play_Attr_Get_Thumb_Draw_Width_Hdmi();
		draw_lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Hdmi();
	}
	else {	// E_BF_PLAY_COMMON_DISP_DEVICE_LCD
		draw_width = BF_Play_Attr_Get_Thumb_Draw_Width_Lcd();
		draw_lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Lcd();
	}
	
	draw_g_width = draw_width;
	if (draw_g_width % 32) {
		draw_g_width = ((draw_g_width >> 5) << 5) + 32;
	}
	
	/* set destination parameter */
	dst_addrs = movie_work_top_img_2_dst;
	BF_Play_Macro_Wrap_Set_Size_Addr( &dst, &dst_addrs, draw_width, draw_lines, draw_g_width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );
	
	/* convert from YCbCr420 to YCbCr420 */
	ret = BF_Play_Macro_Wrap_Rotate_Convert_Image( src, top_image_format, &dst, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_OFF );
	if (ret != 0) {
		BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Convert_Size err on play_index_movie. ret=%d\n", ret));
		return -1;
	}
	
	offset_index = index_num % gBF_Play_Index_Thumbnail_Config.bThumbPerLine;
	if (offset_index >= D_BF_PLAY_INDEX_NUM_LINE_THUMB) {
		return -1;
	}

	if (disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_HDMI) {
		image_addrs.addr_a = work_addr_size->addrs.addr_a + g_bf_play_index_addr_hd[index_num].y_addr;
		image_addrs.addr_b = work_addr_size->addrs.addr_b + g_bf_play_index_addr_hd[index_num].c_addr;
		BF_Play_Macro_Wrap_Copy_Image(dst.addrs.addr_a, dst.addrs.addr_b, draw_g_width, 
										image_addrs.addr_a, image_addrs.addr_b, WIDTH_FHD, 
										draw_width, draw_lines,
										E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, 
										E_BF_PLAY_MACRO_WRAP_USED_2CH);
	}
	else {	// E_BF_PLAY_COMMON_DISP_DEVICE_LCD
		image_addrs.addr_a = work_addr_size->addrs.addr_a + g_bf_play_index_addr_lcd[index_num].y_addr;
		image_addrs.addr_b = work_addr_size->addrs.addr_b + g_bf_play_index_addr_lcd[index_num].c_addr;
		BF_Play_Macro_Wrap_Copy_Image(dst.addrs.addr_a, dst.addrs.addr_b, draw_g_width, 
										image_addrs.addr_a, image_addrs.addr_b, work_addr_size->size.global_width, 
										draw_width, draw_lines,
										E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, 
										E_BF_PLAY_MACRO_WRAP_USED_2CH);
	}
	return ret;
#else // PORTING
	UINT32											file_id = 0;
	INT64											value;
	CHAR											movie_ext[4] = "MP4";
	CHAR 											file_path[256] = {'\0'};
	UCHAR											video_dec_id = 0;
	UINT32											header_addr = 0;
	UINT32											header_size = 1;
	T_FJ_PLAYBACK_MEMINFO							mem_info;
	T_FJ_MUX_DEMUX_COMMON_MEMORY_CONFIG				mem_config;
	T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM				video_config;
	T_FJ_DEMUX_HEADER_INFO							demux_header_info;
	UINT32											read_stream_num;
	T_FJ_DEMUX_STREAM_PARAM							demux_stream_param;
	T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM				video_stream_param[120];
	T_FJ_MOVIE_PLAY_VIDEO_YUV_INFO					decode_yuv_info;
	T_FJ_PLAYBACK_CONVERT							cnv_info;
	UINT32											disp_width;
	UINT32											disp_line;
	UINT32											g_y_width;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR					src_size_addr;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR					dst_size_addr;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR					image_size_addr;
	INT32											prev_file_index = 0;
	INT32											i;

	prev_file_index = FJ_GetCurrentImage();

	FJ_SetCurrentImage( ( INT32 )disp_index );

	BF_Dcf_If_Get_Index_File_Path( disp_index, ( const char * )&movie_ext[0], file_path );

	memset( &mem_info, 0, sizeof( mem_info ) );
	FJ_StillPlayback_GetMemInfo( &mem_info );

	file_id = FJ_Fs_Open( file_path, FJ_FS_OPEN_RDWR );

	if( file_id == 0 ) {
		FJ_VideoDecoder_Disconnect( video_dec_id, disp_device, FALSE );
		FJ_VideoDecoder_Close( video_dec_id );
		FJ_Demux_Finalize( file_id );
		FJ_Mux_Demux_Common_Memory_Finalize();
		FJ_Fs_Close( file_id );

		return -1;
	}

	FJ_Fs_Lseek64( ( ULONG )file_id, ( INT64 )0, ( USHORT )FJ_FS_SEEK_SET, ( INT64 * )&value );

	FJ_GetMem( FJ_HOST_MP4_DEMUXER_PARSED_MEM_LOCATION, &header_addr, &header_size );

	mem_config.header_mem_addr	= header_addr;
	mem_config.header_mem_size	= header_size;
	mem_config.muxing_mem_addr	= 0;
	mem_config.muxing_mem_size	= 0;
	mem_config.muxing_file_num	= 0;
	FJ_Mux_Demux_Common_Memory_Initialize( &mem_config );

	FJ_Demux_Initialize( file_id, &demux_header_info );

	FJ_Demux_VideoSetup( file_id, 0LL, E_FJ_DEMUX_DIRECTION_FWD, E_FJ_DEMUX_SPEED_1X );

	if( demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H264 ) {
		FJ_VideoDecoder_Open( E_FJ_VIDEO_DEC_H264, &video_dec_id );
	}
	else if( demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H265 ) {
		FJ_VideoDecoder_Open( E_FJ_VIDEO_DEC_H265, &video_dec_id );
	}
	else {
		FJ_VideoDecoder_Disconnect( video_dec_id, disp_device, FALSE );
		FJ_VideoDecoder_Close( video_dec_id );
		FJ_Demux_Finalize( file_id );
		FJ_Mux_Demux_Common_Memory_Finalize();
		FJ_Fs_Close( file_id );

		return -1;
	}

	FJ_VideoDecoder_Connect( video_dec_id, disp_device, FALSE );

	FJ_VideoPlayback_VideoInit();

	if( demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H264 ) {
		video_config.codec_type		= FJ_MOVIE_PLAY_CODEC_H264;
	}
	else if( demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H265 ) {
		video_config.codec_type		= FJ_MOVIE_PLAY_CODEC_H265;
	}
	else {
		FJ_VideoDecoder_Disconnect( video_dec_id, disp_device, FALSE );
		FJ_VideoDecoder_Close( video_dec_id );
		FJ_Demux_Finalize( file_id );
		FJ_Mux_Demux_Common_Memory_Finalize();
		FJ_Fs_Close( file_id );

		return -1;
	}

	play_index_get_demux_video_size( demux_header_info.stream_width, demux_header_info.stream_height, &video_config.image_size );

	video_config.scan_mode			= FJ_MOVIE_PLAY_VIDEO_SCAN_MODE_PROGRESSIVE;
	video_config.conceal_error		= FJ_MOVIE_PLAY_VIDEO_CONCEAL_ERROR_SKIP;
	video_config.time_scale			= demux_header_info.video_time_scale;
	video_config.time_delta			= demux_header_info.video_time_delta;
	video_config.trick				= TRUE;
	video_config.compatibility_mode	= FJ_MOVIE_PLAY_OFF;
	FJ_VideoPlayback_SetVideoConfig( &video_config );

	read_stream_num = demux_header_info.offset_of_start_frame + 1;

	if( demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H265 ) {
		read_stream_num++;
	}

	FJ_Demux_StreamReadSync( file_id, demux_header_info.video_codec, read_stream_num, &demux_stream_param );

	for( i = 0; i < demux_stream_param.stream_num; i++ ) {
		video_stream_param[i].stream_end_flag 		= demux_stream_param.stream_end_flag[i];
		video_stream_param[i].stss					= demux_stream_param.stss[i];
		video_stream_param[i].stream_size 			= demux_stream_param.stream_size[i];
		video_stream_param[i].stream_address		= demux_stream_param.stream_address[i];
		video_stream_param[i].pts 					= demux_stream_param.pts[i];
		video_stream_param[i].dts 					= demux_stream_param.dts[i];
	}

	FJ_VideoPlayback_StartIFrameDecoding( !disp_device, 0LL, read_stream_num, video_stream_param, &decode_yuv_info );

	cnv_info.src_img.pfmt				= FJ_IMG_PFMT_VIDEO;
	cnv_info.src_img.addr.AddressY		= decode_yuv_info.y_addr;
	cnv_info.src_img.addr.AddressCb		= decode_yuv_info.cbcr_addr;
	cnv_info.src_img.addr.AddressCr		= decode_yuv_info.cbcr_addr;
	cnv_info.src_img.g_y_width			= decode_yuv_info.g_width;
	cnv_info.src_img.g_c_width			= decode_yuv_info.g_width;
	cnv_info.src_img.width				= decode_yuv_info.width;
	cnv_info.src_img.lines				= decode_yuv_info.lines;
	cnv_info.dst_img.pfmt				= FJ_IMG_PFMT_YCC420;

	if( disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_LCD ) {
		cnv_info.dst_img.addr.AddressY	= mem_info.decoded_output_yuv_main.top_addr;
		disp_width = WIDTH_VGA;
		disp_line = LINES_VGA;
	}
	else {
		cnv_info.dst_img.addr.AddressY	= mem_info.decoded_output_yuv_main.top_addr;
		disp_width = WIDTH_FHD;
		disp_line = LINES_FHD;
	}

	if( 0 != ( disp_width % 8 ) ) {
		g_y_width = ( ( disp_width / 8 ) + 1 ) * 8;
	}
	else {
		g_y_width = disp_width;
	}

	cnv_info.dst_img.addr.AddressCb		= cnv_info.dst_img.addr.AddressY + g_y_width * disp_line;
	cnv_info.dst_img.addr.AddressCr		= cnv_info.dst_img.addr.AddressY + g_y_width * disp_line;
	cnv_info.dst_img.g_y_width			= g_y_width;
	cnv_info.dst_img.g_c_width			= g_y_width;
	cnv_info.dst_img.width				= disp_width;
	cnv_info.dst_img.lines				= disp_line;
	cnv_info.resize_mode				= FJ_IMG_RESIZE_MODE_BILINEAR;
	cnv_info.rotate_deg					= FJ_IMG_ROTATE_DEGREE_0;

	FJ_StillPlayback_ImageConvert( cnv_info );

	src_size_addr.size.width = cnv_info.dst_img.width;
	src_size_addr.size.lines = cnv_info.dst_img.lines;
	src_size_addr.size.global_width = cnv_info.dst_img.g_y_width;
	src_size_addr.size.enc_width = BF_Play_Common_RoundUp_Bit_16( src_size_addr.size.width );
	src_size_addr.addrs.addr_a = cnv_info.dst_img.addr.AddressY;
	src_size_addr.addrs.addr_b = cnv_info.dst_img.addr.AddressCb;
	src_size_addr.addrs.addr_c = cnv_info.dst_img.addr.AddressCr;

	if( disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_LCD ) {
		dst_size_addr.size.width = BF_Play_Attr_Get_Thumb_Draw_Width_Lcd();
		dst_size_addr.size.lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Lcd();
		dst_size_addr.size.global_width = dst_size_addr.size.width;

		if( dst_size_addr.size.global_width % 32 ) {
			dst_size_addr.size.global_width = ( ( dst_size_addr.size.global_width >> 5 ) << 5 ) + 32;
		}

		dst_size_addr.size.enc_width = BF_Play_Common_RoundUp_Bit_16( dst_size_addr.size.width );
		dst_size_addr.addrs.addr_a = mem_info.decoded_resize_yuv.top_addr;
		dst_size_addr.addrs.addr_b = dst_size_addr.addrs.addr_a + dst_size_addr.size.global_width * dst_size_addr.size.lines;
		dst_size_addr.addrs.addr_c = dst_size_addr.addrs.addr_b;

		image_size_addr.addrs.addr_a = work_addr_size->addrs.addr_a + g_bf_play_index_addr_lcd[index_num].y_addr;
		image_size_addr.addrs.addr_b = work_addr_size->addrs.addr_b + g_bf_play_index_addr_lcd[index_num].c_addr;
		image_size_addr.addrs.addr_c = image_size_addr.addrs.addr_b;

		BF_Play_Macro_Wrap_Set_Size_Addr( &image_size_addr, &image_size_addr.addrs,
				BF_Play_Attr_Get_Thumb_Draw_Width_Lcd(), BF_Play_Attr_Get_Thumb_Draw_Lines_Lcd(), WIDTH_VGA,
				E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR );
	}
	else {
		dst_size_addr.size.width = BF_Play_Attr_Get_Thumb_Draw_Width_Hdmi();
		dst_size_addr.size.lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Hdmi();
		dst_size_addr.size.global_width = dst_size_addr.size.width;

		if( dst_size_addr.size.global_width % 32 ) {
			dst_size_addr.size.global_width = ( ( dst_size_addr.size.global_width >> 5 ) << 5 ) + 32;
		}

		dst_size_addr.size.enc_width = BF_Play_Common_RoundUp_Bit_16( dst_size_addr.size.width );
		dst_size_addr.addrs.addr_a = mem_info.decoded_resize_yuv.top_addr;
		dst_size_addr.addrs.addr_b = dst_size_addr.addrs.addr_a + dst_size_addr.size.global_width * dst_size_addr.size.lines;
		dst_size_addr.addrs.addr_c = dst_size_addr.addrs.addr_b;

		image_size_addr.addrs.addr_a = work_addr_size->addrs.addr_a + g_bf_play_index_addr_hd[index_num].y_addr;
		image_size_addr.addrs.addr_b = work_addr_size->addrs.addr_b + g_bf_play_index_addr_hd[index_num].c_addr;
		image_size_addr.addrs.addr_c = image_size_addr.addrs.addr_b;

		BF_Play_Macro_Wrap_Set_Size_Addr( &image_size_addr, &image_size_addr.addrs,
				BF_Play_Attr_Get_Thumb_Draw_Width_Hdmi(), BF_Play_Attr_Get_Thumb_Draw_Lines_Hdmi(), BF_Play_Attr_Get_Thumb_Width_Hdmi(),
				E_BF_PLAY_MACRO_WRAP_SET_FORM_ADDR );
	}

	BF_Play_Macro_Wrap_Rotate_Convert_Image( src_size_addr, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420,
			&dst_size_addr, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420,
			E_BF_DCF_IF_ROTATE_0, E_BF_PLAY_MACRO_WRAP_ADJUST_ON );

	BF_Play_Macro_Wrap_Make_Image_With_Back( dst_size_addr, image_size_addr, E_BF_PLAY_MACRO_WRAP_PFMT_YCC420 );

	FJ_VideoDecoder_Disconnect( video_dec_id, disp_device, FALSE );
	FJ_VideoDecoder_Close( video_dec_id );
	FJ_Demux_Finalize( file_id );
	FJ_Mux_Demux_Common_Memory_Finalize();
	FJ_Fs_Close( file_id );
	FJ_SetCurrentImage( prev_file_index );

	return 0;
#endif // PORTING
}
// --- REMOVE_DCF_EXIF END ---

#if 0
/**
 * @brief		Each fileinfo set process for Thumbnail
 * @param		o:USHORT index_list
 * @return		None
 * @note		None
 * @attention	None
 */
static VOID play_index_set_list( ULONG index_list[D_BF_PLAY_INDEX_NUMBER_OF_THUMB] )
{
	INT32	file_no;
	USHORT	first_index = g_bf_play_index_list.first_index;
	ULONG	current_dir = (BF_Play_Attr_Get_Image_Index()) & 0xFFFF0000;
	T_BF_DCF_IF_FILE_INFO index_file_info;
	E_BF_DCF_IF_FILE_TYPE file_type;

	for (file_no = 0; file_no < g_bf_play_index_list.thumb_count; file_no++) {
		file_type = E_BF_DCF_IF_FILE_TYPE_NOTHING;

		do {
			index_list[file_no] = current_dir + first_index;
			first_index++;

			if( D_BF_FS_USER_CUSTOM_OK != BF_Dcf_If_Get_File_Info( index_list[file_no], &index_file_info ) ) {
				break;
			}

			file_type = BF_Dcf_If_Convert_File_Type( index_file_info.file_type );
		}
		while( file_type != E_BF_DCF_IF_FILE_TYPE_JPG && file_type != E_BF_DCF_IF_FILE_TYPE_MP4 && file_type != E_BF_DCF_IF_FILE_TYPE_MOV );
	}
}
#endif

/**
 * @brief		Thumbnail display process for Thumbnail photo or movie tab
 * @param		i:T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* work_addr_size
 * @param		i:USHORT index_list
 * @param		i:USHORT single_page
 * @param		i:E_BF_PLAY_COMMON_DISP_DEVICE disp_device
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
static FJ_ERR_CODE play_index_main( T_BF_PLAY_MACRO_WRAP_SIZE_ADDR* work_addr_size, ULONG index_list[D_BF_PLAY_INDEX_NUMBER_OF_THUMB], USHORT single_page, E_BF_PLAY_COMMON_DISP_DEVICE disp_device )
{
// --- REMOVE_DCF_EXIF BEGIN ---
	INT32	ret = 0;
	INT32	thumb_no;
	E_BF_DCF_IF_FILE_TYPE	file_type;

	/***********************/
	/* set parameter       */
	/***********************/
	/* initialize */
	g_bf_play_index_list.first_index = index_list[0];
	g_bf_play_index_list.last_index  = index_list[g_bf_play_index_list.thumb_count - 1];
	memset( g_bf_play_index_error_list, 0x00, sizeof(g_bf_play_index_error_list) );
	memset( g_bf_play_index_file_info,  0x00, sizeof(g_bf_play_index_file_info) );
	/************************************/
	/* thumbnail data load main process */
	/************************************/
	if (single_page == 0) {
		for (thumb_no = 0; thumb_no < g_bf_play_index_list.thumb_count; thumb_no++) {

				/* get file info */
				BF_Dcf_If_Get_File_Info( index_list[thumb_no], &g_bf_play_index_file_info[thumb_no] );
				file_type = BF_Dcf_If_Convert_File_Type(g_bf_play_index_file_info[thumb_no].file_type);
				/* check file type */
				switch (file_type) {
					case E_BF_DCF_IF_FILE_TYPE_JPG:
						ret = play_index_jpeg( thumb_no, index_list[thumb_no], work_addr_size, &g_bf_play_index_file_info[thumb_no], disp_device );
						break;

					case E_BF_DCF_IF_FILE_TYPE_WAV:
						ret = play_index_wave( thumb_no, work_addr_size, disp_device );
						break;

					case E_BF_DCF_IF_FILE_TYPE_RAW:
						ret = 0;
						break;

					case E_BF_DCF_IF_FILE_TYPE_AVI:		// Fallthrough
					case E_BF_DCF_IF_FILE_TYPE_MP4:		// Fallthrough
					case E_BF_DCF_IF_FILE_TYPE_MOV:
						ret = play_index_movie( thumb_no, index_list[thumb_no], work_addr_size, disp_device, file_type );
						break;

					case E_BF_DCF_IF_FILE_TYPE_TIF:
						ret = -1;
						break;

					default:
						ret = -1;
						break;
				}
			if (ret != 0) {
				/* set error flg */
				ret = play_index_illigal( thumb_no, work_addr_size );
				g_bf_play_index_error_list[thumb_no] = D_BF_PLAY_INDEX_THUMB_ERROR_ON;
			}
		}
	}

// --- REMOVE_DCF_EXIF END ---
	return FJ_ERR_OK;
}

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/
/**
 * @brief		Thumbnail image display process for Thumbnail
 * @param		None
 * @return		FJ_ERR_CODE
 * @note		None
 * @attention	None
 */
FJ_ERR_CODE BF_Play_Index_Manage(VOID)
{
	UINT32									thumb_info_count = 0;
	UCHAR									thumb_total_count = 0;
	ULONG									work_addr = 0;
	USHORT									gap_width;
	USHORT									gap_lines;
	USHORT									single_page = 0;
	ULONG									index_list[D_BF_PLAY_INDEX_NUMBER_OF_THUMB];
	UCHAR									effective_device;
	USHORT									draw_width;
	USHORT									draw_lines;
	USHORT									lines = gBF_Play_Index_Thumbnail_Config.bThumbPerLine;
	USHORT									colmn = gBF_Play_Index_Thumbnail_Config.bThumbPerColumn;
	T_BF_PLAY_MACRO_WRAP_SIZE				hdmi_output_size;
	FJ_T_PLAYBACK_THUMBNAILINDEX_INFO		thumb_info[D_BF_PLAY_INDEX_NUMBER_OF_THUMB] = {{0}};
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR			hd_param_src, hd_param_dst;
	T_BF_PLAY_MACRO_WRAP_SIZE_ADDR			vga_param_src, vga_param_dst;

	memset( &hdmi_output_size, 0x00, sizeof(T_BF_PLAY_MACRO_WRAP_SIZE) );

	effective_device = BF_Play_Macro_Wrap_Get_Effective_Device();	// LCD/HD/TV connect status get
	
	// check parameters
	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_HDMI & effective_device) {	// HDMI enable
		draw_width = BF_Play_Attr_Get_Thumb_Draw_Width_Hdmi();
		draw_lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Hdmi();

		if (BF_Play_Macro_Wrap_Get_Hdmi_Output_Size(&hdmi_output_size) != FJ_ERR_OK){
			BF_Debug_Print_Error(("BF_Play_Macro_Wrap_Get_Input_Size() Error\n"));
			return FJ_ERR_NG;
		}

		gap_width = gBF_Play_Index_Thumbnail_Config.bFirstHGap_HD + (gBF_Play_Index_Thumbnail_Config.bHGap_HD * (lines - 1)) + (draw_width * lines);
		gap_lines = gBF_Play_Index_Thumbnail_Config.bFirstVGap_HD + (gBF_Play_Index_Thumbnail_Config.bVGap_HD * (colmn - 1)) + (draw_lines * colmn);
		
		if ((gap_width > hdmi_output_size.width) || (gap_lines > hdmi_output_size.lines)) {
			BF_Debug_Print_Error(("ThumbnailIndex param error! set again to FJ_ConfigThumbnail. gap_width: %d, gap_lines: %d\n", gap_width, gap_lines));
			return FJ_ERR_NG;
		}
		
		work_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)SDRAM_ADR_PLAY_INDEX_VIEW_IMG_AREA_HD );		/* pgr0080 */
		BF_Play_Macro_Wrap_Set_Size_Addr( &hd_param_src, &work_addr,
				BF_Play_Attr_Get_Thumb_Width_Hdmi(), BF_Play_Attr_Get_Thumb_Height_Hdmi(),
				BF_Play_Attr_Get_Thumb_Width_Hdmi(),
				E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );
	}
	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_LCD & effective_device) {		// VLD or TV enable
		draw_width = BF_Play_Attr_Get_Thumb_Draw_Width_Lcd();
		draw_lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Lcd();
		
		gap_width = gBF_Play_Index_Thumbnail_Config.bFirstHGap + (gBF_Play_Index_Thumbnail_Config.bHGap * (lines - 1)) + (draw_width * lines);
		gap_lines = gBF_Play_Index_Thumbnail_Config.bFirstVGap + (gBF_Play_Index_Thumbnail_Config.bVGap * (colmn - 1)) + (draw_lines * colmn);
		
		if ((gap_width > D_BF_PLAY_INDEX_DRAWING_WIDTH_LCD) || (gap_lines > D_BF_PLAY_INDEX_DRAWING_LINES_LCD)) {
			BF_Debug_Print_Error(("ThumbnailIndex param error! set again to FJ_ConfigThumbnail\n"));
			return FJ_ERR_NG;
		}
		
		work_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( (ULONG)SDRAM_ADR_PLAY_INDEX_VIEW_IMG_AREA_LCD );	/* pgr0080 */
		BF_Play_Macro_Wrap_Set_Size_Addr( &vga_param_src, &work_addr, WIDTH_VGA, LINES_VGA, WIDTH_VGA, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );
	}

	if (play_index_set_current( index_list ) != FJ_ERR_OK) {	// set current index
		return FJ_ERR_NG;
	}

	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_HDMI & effective_device) {	// HDMI enable
		play_index_init( &hd_param_src, E_BF_PLAY_COMMON_DISP_DEVICE_HDMI );

		play_index_set_pos( E_BF_PLAY_COMMON_DISP_DEVICE_HDMI );

		if (play_index_main( &hd_param_src, index_list, single_page, E_BF_PLAY_COMMON_DISP_DEVICE_HDMI ) != FJ_ERR_OK) {
			return FJ_ERR_NG;
		}
	}
	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_LCD & effective_device) {		// VLD enable
		play_index_init( &vga_param_src, E_BF_PLAY_COMMON_DISP_DEVICE_LCD );

		play_index_set_pos( E_BF_PLAY_COMMON_DISP_DEVICE_LCD );

		if (play_index_main( &vga_param_src, index_list, single_page, E_BF_PLAY_COMMON_DISP_DEVICE_LCD ) != FJ_ERR_OK) {
			return FJ_ERR_NG;
		} 
	}

	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_HDMI & effective_device) {	// HDMI enable
		work_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Disp_YUV_HDMI_Addr() );
		BF_Play_Macro_Wrap_Set_Size_Addr( &hd_param_dst, &work_addr, hdmi_output_size.width, hdmi_output_size.lines, hdmi_output_size.width, E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );

		BF_Play_Macro_Wrap_Copy_Image(hd_param_src.addrs.addr_a, hd_param_src.addrs.addr_b, hd_param_src.size.global_width, 			/* pgr0872 */	/* pgr0000 */
										   hd_param_dst.addrs.addr_a, hd_param_dst.addrs.addr_b, hd_param_dst.size.global_width, 
										   hd_param_dst.size.width, hd_param_dst.size.lines, 											
										   E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, 
										   E_BF_PLAY_MACRO_WRAP_USED_2CH);

		BF_Play_Macro_Wrap_Display_Hdmi( hd_param_dst );
	}

	if (E_BF_PLAY_MACRO_WRAP_EFFECTIVE_LCD & effective_device) {		// LCD enable
		work_addr = M_SDRAM_MAP_COMMON_ADR_LOG2PHY( BF_Sdram_Map_Play_Get_Vga_Out_Addr() );
		BF_Play_Macro_Wrap_Set_Size_Addr( &vga_param_dst, &work_addr, WIDTH_VGA, LINES_VGA, WIDTH_VGA,
			E_BF_PLAY_MACRO_WRAP_SET_FORM_ULONG );

		BF_Play_Macro_Wrap_Copy_Image(vga_param_src.addrs.addr_a, vga_param_src.addrs.addr_b, vga_param_src.size.global_width, 		/* pgr0872 */	/* pgr0000 */
										   vga_param_dst.addrs.addr_a, vga_param_dst.addrs.addr_b, vga_param_dst.size.global_width, 
										   vga_param_dst.size.width, vga_param_dst.size.lines, 											
										   E_BF_PLAY_MACRO_WRAP_PFMT_YCC420, 
										   E_BF_PLAY_MACRO_WRAP_USED_2CH);

		BF_Play_Macro_Wrap_Display_Lcd( vga_param_dst );
	}

	// set thumbnail info
	thumb_total_count = (lines * colmn);
	for(thumb_info_count = 0; thumb_info_count < thumb_total_count; thumb_info_count++) {
		thumb_info[thumb_info_count].thumb_index = (INT32)index_list[thumb_info_count];
		thumb_info[thumb_info_count].thumb_error = (CHAR)g_bf_play_index_error_list[thumb_info_count];
	}

	// call back
	FJ_Host_ThumbnailIndexCB( thumb_info, g_bf_play_index_list.thumb_count);

	return FJ_ERR_OK;
}

VOID * BF_Play_Index_Get_Pos( E_BF_PLAY_COMMON_DISP_DEVICE disp_device )
{
	if( disp_device == E_BF_PLAY_COMMON_DISP_DEVICE_LCD ) {
		return &g_bf_play_index_pos_lcd;
	}
	else {
		return &g_bf_play_index_pos_hd;
	}
}

