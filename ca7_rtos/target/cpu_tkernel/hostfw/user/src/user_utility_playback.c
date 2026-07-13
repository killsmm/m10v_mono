/**
 * @file		user_utility.c
 * @brief		None
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2018 Socionext Inc.</I></B>
 */

#include "user.h"
#include "fj_dcf.h"
#include "fj_demux.h"
#include "fj_filesystem.h"
#include "fj_memory.h"
#include "fj_mux_demux_common.h"
#include "fj_playback.h"
#include "audio_codec_wrapper.h"
#include "user_utility_disp.h"
#include "category_parameter.h"
#include "play_attr.h"
#include "sdram_map_cache_define.h"
#include "osd_shape_comm.h"
#include "fj_disp.h"

/*==============================================================================*/
/* Definition																	*/
/*==============================================================================*/


/*==============================================================================*/
/* Macro																		*/
/*==============================================================================*/

#define D_USER_UTILITY_PLAYBACK_JPEG_WIDTH_MAX			( 6000 )
#define D_USER_UTILITY_PLAYBACK_JPEG_LINES_MAX			( 3000 )

#define D_USER_UTILITY_PLAYBACK_VIDEO_STREAM_MAX		( 120 )
#define D_USER_UTILITY_PLAYBACK_AUDIO_STREAM_MAX		( 24 )

/*==============================================================================*/
/* Enumeration																	*/
/*==============================================================================*/

typedef enum {
	USER_UTILITY_PLAYBACK_VIDEO_OFF,
	USER_UTILITY_PLAYBACK_VIDEO_PLAY,
	USER_UTILITY_PLAYBACK_VIDEO_TRICK_PLAY,
	USER_UTILITY_PLAYBACK_VIDEO_PAUSE,
	USER_UTILITY_PLAYBACK_VIDEO_TRICK_PLAY_PAUSE,
	USER_UTILITY_PLAYBACK_VIDEO_FRAME_BY_FRAME
} USER_UTILITY_PLAYBACK_VIDEO_STATUS;

typedef enum {
	USER_UTILITY_PLAYBACK_AUDIO_OFF,
	USER_UTILITY_PLAYBACK_AUDIO_ON,
	USER_UTILITY_PLAYBACK_AUDIO_INVALID
} USER_UTILITY_PLAYBACK_AUDIO_STATUS;

typedef enum {
	USER_UTILITY_PLAYBACK_THUMBNAIL_STYLE_3X3,
	USER_UTILITY_PLAYBACK_THUMBNAIL_STYLE_4X4,
	USER_UTILITY_PLAYBACK_THUMBNAIL_STYLE_5X5
} USER_UTILITY_PLAYBACK_THUMBNAIL_STYLE;

typedef enum {
	USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_UP,
	USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_DOWN,
	USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_LEFT,
	USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_RIGHT,
} USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_OPERATION;

/*==============================================================================*/
/* Structure																	*/
/*==============================================================================*/

typedef union {
	UINT32		word;

	struct {
		USHORT	obj_index;
		USHORT	dir_index;
	} dir_obj;
} USER_UTILITY_PLAYBACK_DIR_OBJ;

typedef struct {
	UINT32		offset;
	UINT32		size;
	UINT32		g_width;
	UINT32		g_lines;
	UINT32		width;
	UINT32		lines;
	UCHAR		format;
} USER_UTILITY_PLAYBACK_JPEG_INFO;

typedef struct {
	INT32				x_pos;
	INT32				y_pos;
}USER_UTILITY_PLAYBACK_THUMBNAIL_POS;

typedef struct {
	UINT16		position_index;
	UINT16		line_num;
	UINT16		column_num;

	UINT16		total_num;
	UINT16		current_num;

	UINT16		total_page;
	UINT16		current_page;
	UINT16		thumbnail_num;

	UINT16		width;
	UINT16		lines;

	USER_UTILITY_PLAYBACK_THUMBNAIL_POS * position;
	USER_UTILITY_PLAYBACK_DIR_OBJ file_list[5*5];
	USER_UTILITY_PLAYBACK_DIR_OBJ page_list[5*5];
} USER_UTILITY_PLAYBACK_THUMBNAIL_INFO;

/*==============================================================================*/
/* Global Data																	*/
/*==============================================================================*/

static E_SHARE_DISP_IF						g_playback_disp_id = E_SHARE_DISP_IF_HDMI;
static USER_UTILITY_PLAYBACK_VIDEO_STATUS	g_playback_video_status = USER_UTILITY_PLAYBACK_VIDEO_OFF;
static USER_UTILITY_PLAYBACK_AUDIO_STATUS	g_playback_audio_status = USER_UTILITY_PLAYBACK_AUDIO_OFF;

static UINT32								g_playback_file_id;
static CHAR									g_playback_file_name[256] = {'\0'};
static USHORT								g_playback_file_type;

static T_FJ_PLAYBACK_MEMINFO				g_mem_info;
static USER_UTILITY_PLAYBACK_JPEG_INFO		g_jpeg_info;
static UINT32								g_jpeg_addr;

static UCHAR								g_video_dec_id;
static UCHAR								g_audio_dec_id;
static UCHAR								g_audio_out_id;

static BOOL									g_video_end_flag = FALSE;
static BOOL									g_audio_end_flag = FALSE;
static BOOL									g_video_error_flag = FALSE;

static T_FJ_DEMUX_HEADER_INFO				g_demux_header_info;
static ULLONG								g_demux_pts;

static USER_UTILITY_PLAYBACK_THUMBNAIL_INFO	g_thumbnail_info;
static BOOL									g_thumbnail_status = FALSE;
static BOOL									g_thumbnail_enter_pressed = FALSE;

/*==============================================================================*/
/* Local Function Definition													*/
/*==============================================================================*/

static INT32 user_utility_playback_get_demux_video_size( UINT32 width, UINT32 lines, E_FJ_MOVIE_PLAY_VIDEO_SIZE * image_size );
static INT32 user_utility_playback_get_thumbnail_info( UINT16 position_index );
static INT32 user_utility_playback_get_thumbnail_config( USER_UTILITY_PLAYBACK_THUMBNAIL_STYLE thumbnail_style, THUMBNAIL_CONFIGURATION * thumbnail_config );
static INT32 user_utility_playback_set_file_by_number( ULLONG file_number );
static INT32 user_utility_playback_set_file_by_index( UINT32 file_index );
static INT32 user_utility_playback_jpeg_start( VOID );
static INT32 user_utility_playback_jpeg_display( CHAR * file_name, UCHAR rotate );
static INT32 user_utility_playback_jpeg_load( CHAR * file_path );
static INT32 user_utility_playback_video_display( UINT32 start_pts, FJ_DISP_ROTATE_DEGREE rotate );
static INT32 user_utility_playback_video_start( UINT32 start_pts, E_FJ_MOVIE_PLAY_DIRECTION direction, E_FJ_MOVIE_PLAY_SPEED speed, FJ_DISP_ROTATE_DEGREE rotate );
static INT32 user_utility_playback_video_restart( VOID );
static INT32 user_utility_playback_video_trick_play( UINT32 start_pts, E_FJ_MOVIE_PLAY_DIRECTION direction, E_FJ_MOVIE_PLAY_SPEED speed, BOOL pause, BOOL time_search );
static INT32 user_utility_playback_video_pause( VOID );
static INT32 user_utility_playback_video_stop( VOID );
static INT32 user_utility_playback_video_end( VOID );
static INT32 user_utility_playback_thumbnail_start( BOOL is_thumbnail_update, UINT32 image_index, UINT16 position_index );
static INT32 user_utility_playback_thumbnail_display( UINT32 image_index );
static INT32 user_utility_playback_thumbnail_display_rectangle( UINT16 position_index, UINT32 color, UINT32 thickness_pix );
static INT32 user_utility_playback_thumbnail_key_process( USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_OPERATION key_operation );

/*==============================================================================*/
/* Global Function																*/
/*==============================================================================*/

INT32 User_Utility_Playback_Jpeg_Play( ULLONG file_index )
{
	INT32 ret;

	if( ( g_playback_disp_id != E_SHARE_DISP_IF_LCD )
			&& ( g_playback_disp_id != E_SHARE_DISP_IF_HDMI ) ) {
		g_playback_disp_id = E_SHARE_DISP_IF_HDMI;
	}

	ret = user_utility_playback_set_file_by_number( file_index );
	UPRINTF_ERR( ret );

	if( ret != OK ) {
		return NG;
	}

	ret = user_utility_playback_jpeg_start();
	UPRINTF_ERR( ret );

	if( ret != OK ) {
		return NG;
	}

	return OK;
}

INT32 User_Utility_Playback_Video_Play( UINT32 second )
{
	INT32 ret;

	switch( g_playback_video_status ) {
		case USER_UTILITY_PLAYBACK_VIDEO_OFF:
			if( g_playback_file_type == FJ_FILE_TYPE_JPEG ) {
				return NG;
			}

			ret = user_utility_playback_video_start( 0, E_FJ_MOVIE_PLAY_DIRECTION_FWD, E_FJ_MOVIE_PLAY_SPEED_1X, FJ_DISP_ROTATE_DEGREE_0 );
			UPRINTF_ERR( ret );
			break;

		case USER_UTILITY_PLAYBACK_VIDEO_PLAY:
			ret = User_Utility_Playback_Video_Play_Spec_Time( second );
			UPRINTF_ERR( ret );
			break;

		case USER_UTILITY_PLAYBACK_VIDEO_PAUSE:
			ret = user_utility_playback_video_restart();
			UPRINTF_ERR( ret );
			break;

		case USER_UTILITY_PLAYBACK_VIDEO_TRICK_PLAY:
		case USER_UTILITY_PLAYBACK_VIDEO_TRICK_PLAY_PAUSE:
		case USER_UTILITY_PLAYBACK_VIDEO_FRAME_BY_FRAME:
			ret = user_utility_playback_video_trick_play(g_demux_pts, E_FJ_MOVIE_PLAY_DIRECTION_FWD, E_FJ_MOVIE_PLAY_SPEED_1X, FALSE, FALSE);
			UPRINTF_ERR( ret );
			break;

		default:
			return NG;
			break;
	}

	if( ret != OK ) {
		user_utility_playback_video_end();
		return NG;
	}

	return OK;
}

INT32 User_Utility_Playback_Video_Pause( VOID )
{
	INT32 ret;

	if( g_playback_video_status == USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	switch( g_playback_video_status ) {
		case USER_UTILITY_PLAYBACK_VIDEO_PLAY:
		case USER_UTILITY_PLAYBACK_VIDEO_TRICK_PLAY:
			ret = user_utility_playback_video_pause();
			UPRINTF_ERR( ret );
			break;

		case USER_UTILITY_PLAYBACK_VIDEO_PAUSE:
		case USER_UTILITY_PLAYBACK_VIDEO_TRICK_PLAY_PAUSE:
			ret = User_Utility_Playback_Video_Play( 0 );
			UPRINTF_ERR( ret );
			break;

		case USER_UTILITY_PLAYBACK_VIDEO_FRAME_BY_FRAME:
			return OK;
			break;

		default:
			return NG;
			break;
	}

	if( ret != OK ) {
		user_utility_playback_video_end();
		return NG;
	}

	return OK;
}

INT32 User_Utility_Playback_Video_Stop( VOID )
{
	INT32 ret;

	if( g_playback_video_status == USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	ret = user_utility_playback_video_stop();
	UPRINTF_ERR( ret );

	ret = user_utility_playback_video_end();
	UPRINTF_ERR( ret );

	return OK;
}

INT32 User_Utility_Playback_Video_Fast_Forward( E_FJ_MOVIE_PLAY_SPEED speed )
{
	INT32 ret;

	if( g_playback_video_status == USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	ret = user_utility_playback_video_trick_play(g_demux_pts, E_FJ_MOVIE_PLAY_DIRECTION_FWD, speed, FALSE, FALSE);
	UPRINTF_ERR( ret );

	if( ret != OK ) {
		user_utility_playback_video_end();
		return NG;
	}

	return OK;
}

INT32 User_Utility_Playback_Video_Fast_Rewind( E_FJ_MOVIE_PLAY_SPEED speed )
{
	INT32 ret;

	if( g_playback_video_status == USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	ret = user_utility_playback_video_trick_play(g_demux_pts, E_FJ_MOVIE_PLAY_DIRECTION_RWD, speed, FALSE, FALSE);
	UPRINTF_ERR( ret );

	if( ret != OK ) {
		user_utility_playback_video_end();
		return NG;
	}

	return OK;
}

INT32 User_Utility_Playback_Video_Frame_By_Frame( VOID )
{
	INT32 ret;

	if( g_playback_video_status == USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	if( g_demux_pts >= g_demux_header_info.video_play_last_pts ) {
		return NG;
	}

	g_demux_pts += g_demux_header_info.video_time_delta;
	ret = user_utility_playback_video_trick_play( g_demux_pts, E_FJ_MOVIE_PLAY_DIRECTION_FWD, E_FJ_MOVIE_PLAY_SPEED_1X, TRUE, TRUE );
	UPRINTF_ERR( ret );

	if( ret != OK ) {
		user_utility_playback_video_end();
		return NG;
	}

	return OK;
}

INT32 User_Utility_Playback_Video_Play_Spec_Time( UINT32 second )
{
	INT32 ret;
	ULLONG start_pts = 0;

	if( g_playback_video_status == USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	start_pts = ( g_demux_header_info.video_time_scale * second / g_demux_header_info.video_time_delta ) * g_demux_header_info.video_time_delta;

	if( start_pts > g_demux_header_info.video_play_last_pts ) {
		start_pts = g_demux_header_info.video_play_last_pts - g_demux_header_info.video_time_delta;
	}

	ret = user_utility_playback_video_trick_play( start_pts, E_FJ_MOVIE_PLAY_DIRECTION_FWD, E_FJ_MOVIE_PLAY_SPEED_1X, FALSE, TRUE );
	UPRINTF_ERR( ret );

	if( ret != OK ) {
		user_utility_playback_video_end();
		return NG;
	}

	return OK;
}

INT32 User_Utility_Playback_Thumbnail_Play( UINT32 thumbnail_style )
{
	INT32							ret;
	USHORT							dir_count = 0;
	USHORT							obj_count = 0;
	USER_UTILITY_PLAYBACK_DIR_OBJ	dir_obj = {0};
	THUMBNAIL_CONFIGURATION			thumbnail_config = {0};
	INT32							i;

	INT32							dir_key;				/* DCF directory key			*/
	INT32							file_key;				/* DCF file key					*/
	SHORT							file_type;				/* DCF file type				*/
	UINT32							attribute;				/* attribute ( file/object )	*/
	INT32							file_dup_status;		/* DCF duplicate status			*/
	USHORT							ext_type;				/* File extension type			*/
	CHAR							ext_str[4] = {'\0'};

	UINT16							total_num = 0;
	UINT16							thumbnail_num_max = 0;	
	USER_UTILITY_PLAYBACK_DIR_OBJ	page_list[5*5];

	if( g_thumbnail_status == TRUE ) {
		return NG;
	}

	switch( thumbnail_style ) {
		case USER_UTILITY_PLAYBACK_THUMBNAIL_STYLE_3X3:
			thumbnail_num_max = 3*3;
			break;

		case USER_UTILITY_PLAYBACK_THUMBNAIL_STYLE_4X4:
			thumbnail_num_max = 4*4;
			break;

		case USER_UTILITY_PLAYBACK_THUMBNAIL_STYLE_5X5:
			thumbnail_num_max = 5*5;
			break;

		default:
			return NG;
	}

	ret = FJ_DirectDcfDirNum( &dir_count );
	UPRINTF_ERR( ret );

	for( i = dir_count; i > 0; i-- ) {
		ret = FJ_SetCurrentDirectory( i );
		UPRINTF_ERR( ret );

		if( ret != FJ_ERR_OK ) {
			continue;
		}

		ret = FJ_Get_Latest_Index( ( INT32 * )&dir_obj );
		UPRINTF_ERR( ret );

		if( ret != FJ_ERR_OK ) {
			continue;
		}

		obj_count = dir_obj.dir_obj.obj_index + 1;
		dir_obj.dir_obj.obj_index = 0;
		break;
	}

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	for( i = 0; i < obj_count; i++ ) {
		dir_obj.dir_obj.obj_index += 1;
		ext_type = 0;

		if( FJ_ERR_OK != FJ_GetDCFInfo( dir_obj.word , &dir_key, &file_key, &file_type, ( INT32 * )&attribute, &file_dup_status ) ) {
			break;
		}

		if( FJ_ERR_OK != FJ_GetDCFDBStrVal( FJ_PARA_DCFDB_EXT_GET_STR, file_type, ext_str ) ) {
			break;
		}

		if( strcmp( ext_str, "JPG" ) == 0 ) {
			if( TRUE == BF_Play_Macro_Wrap_Check_Jpeg_Thumbnail( dir_obj.word ) ) {
				ext_type = FJ_FILE_TYPE_JPEG;
			}
		}
		else if( strcmp( ext_str, "MP4" ) == 0 ) {
			ext_type = FJ_FILE_TYPE_MP4;
		}
		else if( strcmp( ext_str, "MOV" ) == 0 ) {
			ext_type = FJ_FILE_TYPE_MOV;
		}

		if( ext_type == FJ_FILE_TYPE_JPEG || ext_type == FJ_FILE_TYPE_MP4 || ext_type == FJ_FILE_TYPE_MOV ) {
			if( total_num % thumbnail_num_max == 0 ) {
				page_list[total_num / thumbnail_num_max].word = dir_obj.word;
			}
		
			total_num += 1;
		}
	}

	if( total_num == 0 ) {
		return NG;
	}

	g_thumbnail_status = TRUE;

	ret = user_utility_playback_get_thumbnail_config( thumbnail_style, &thumbnail_config );
	UPRINTF_ERR( ret );

	ret = FJ_ConfigThumbIndex( &thumbnail_config );
	UPRINTF_ERR( ret );

	dir_obj.dir_obj.obj_index = 1;
	g_thumbnail_info.current_num = 0;
	g_thumbnail_info.total_num = total_num;
	memcpy( g_thumbnail_info.page_list, page_list, 5*5 * sizeof( USER_UTILITY_PLAYBACK_DIR_OBJ ) );

	ret = user_utility_playback_thumbnail_start( TRUE, dir_obj.word, 0 );
	UPRINTF_ERR( ret );

	return OK;
}

INT32 User_Utility_Playback_Thumbnail_Key_Up( VOID )
{
	INT32 ret;

	if( g_playback_video_status != USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	if( g_thumbnail_status == FALSE ) {
		return NG;
	}

	ret = user_utility_playback_thumbnail_key_process( USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_UP );
	UPRINTF_ERR( ret );

	return OK;
}

INT32 User_Utility_Playback_Thumbnail_Key_Down( VOID )
{
	INT32 ret;

	if( g_playback_video_status != USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	if( g_thumbnail_status == FALSE ) {
		return NG;
	}

	ret = user_utility_playback_thumbnail_key_process( USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_DOWN );
	UPRINTF_ERR( ret );

	return OK;
}

INT32 User_Utility_Playback_Thumbnail_Key_Left( VOID )
{
	INT32 ret;

	if( g_playback_video_status != USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	if( g_thumbnail_status == FALSE ) {
		return NG;
	}

	ret = user_utility_playback_thumbnail_key_process( USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_LEFT );
	UPRINTF_ERR( ret );

	return OK;
}

INT32 User_Utility_Playback_Thumbnail_Key_Right( VOID )
{
	INT32 ret;

	if( g_playback_video_status != USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	if( g_thumbnail_status == FALSE ) {
		return NG;
	}

	ret = user_utility_playback_thumbnail_key_process( USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_RIGHT );
	UPRINTF_ERR( ret );

	return OK;
}

INT32 User_Utility_Playback_Thumbnail_Key_Enter( VOID )
{
	INT32 ret = NG;

	if( g_playback_video_status != USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	if( g_thumbnail_status == FALSE ) {
		return NG;
	}

	if( g_thumbnail_enter_pressed == FALSE ) {
		ret = User_Utility_Disp_OSD_Disable( g_playback_disp_id );
		UPRINTF_ERR( ret );

		ret = user_utility_playback_set_file_by_index( g_thumbnail_info.file_list[g_thumbnail_info.position_index].word );
		UPRINTF_ERR( ret );

		if( ret != OK ) {
			return NG;
		}

		if( g_playback_file_type == FJ_FILE_TYPE_JPEG ) {
			ret = user_utility_playback_jpeg_start();
			UPRINTF_ERR( ret );
		}
		else if( g_playback_file_type == FJ_FILE_TYPE_MP4 ) {
			ret = User_Utility_Playback_Video_Play( 0 );
			UPRINTF_ERR( ret );
		}

		g_thumbnail_enter_pressed = TRUE;
	}
	else {
		ret = user_utility_playback_thumbnail_start( TRUE, g_thumbnail_info.file_list[0].word, g_thumbnail_info.position_index );
		UPRINTF_ERR( ret );

		g_thumbnail_enter_pressed = FALSE;
	}

	if( ret != OK ) {
		return NG;
	}

	return OK;
}

INT32 User_Utility_Playback_Thumbnail_End( VOID )
{
	INT32 ret;

	if( g_thumbnail_status == FALSE ) {
		return NG;
	}

	ret = User_Utility_Disp_OSD_Disable( g_playback_disp_id );
	UPRINTF_ERR( ret );

	ret = User_Utility_Disp_Disable( g_playback_disp_id );
	UPRINTF_ERR( ret );

	g_thumbnail_status = FALSE;
	g_thumbnail_enter_pressed = FALSE;

	return OK;
}

INT32 User_Utility_Playback_Print_File_List( VOID )
{
	INT32							ret;
	USHORT							dir_count = 0;
	USHORT							obj_count = 0;
	USER_UTILITY_PLAYBACK_DIR_OBJ	dir_obj = {0};
	USER_UTILITY_PLAYBACK_DIR_OBJ	dir_obj_v2 = {0};
	INT32							dir_key;				/* DCF directory key			*/
	INT32							file_key;				/* DCF file key					*/
	SHORT							file_type;				/* DCF file type				*/
	UINT32							attribute;				/* attribute ( file/object )	*/
	INT32							file_dup_status;		/* DCF duplicate status			*/
	CHAR							file_name[256] = {'\0'};
	CHAR							ext_str[4] = {'\0'};
	INT32							i;

	ret = FJ_DirectDcfDirNum( &dir_count );
	UPRINTF_ERR( ret );

	for( i = dir_count; i > 0; i-- ) {
		ret = FJ_SetCurrentDirectory( i );
		UPRINTF_ERR( ret );

		if( ret != FJ_ERR_OK ) {
			continue;
		}

		ret = FJ_Get_Latest_Index( ( INT32 * )&dir_obj );
		UPRINTF_ERR( ret );

		if( ret != FJ_ERR_OK ) {
			continue;
		}

		obj_count = dir_obj.dir_obj.obj_index + 1;
		dir_obj_v2.word = dir_obj.word;

		if( dir_obj.dir_obj.obj_index != 0 ) {
			break;
		}
	}

	if( dir_obj.dir_obj.obj_index == 0 ) {
		return NG;
	}

	ret = FJ_GetDCFDBStrVal( FJ_PARA_DCFDB_DIR_GET_STR, dir_obj.word, file_name );
	UPRINTF_ERR( ret );

	UPRINTF( "== %s ==\r\n", file_name );

	for( i = 0; i < 10; i++ ) {
		ret = FJ_GetDCFDBStrVal( FJ_PARA_DCFDB_OBJ_GET_STR, dir_obj.word, file_name );
		UPRINTF_ERR( ret );

		if( ret != FJ_ERR_OK ) {
			break;
		}

		ret = FJ_GetDCFInfo( dir_obj.word, &dir_key, &file_key, &file_type, ( INT32 * )&attribute, &file_dup_status );
		UPRINTF_ERR( ret );

		if( ret != FJ_ERR_OK ) {
			break;
		}

		ret = FJ_GetDCFDBStrVal( FJ_PARA_DCFDB_EXT_GET_STR, file_type, ext_str );
		UPRINTF_ERR( ret );

		if( ret != FJ_ERR_OK ) {
			break;
		}

		UPRINTF( "%s.%s\r\n", file_name, ext_str );

		dir_obj.dir_obj.obj_index--;

		if( dir_obj.dir_obj.obj_index == 0 ) {
			break;
		}
	}

	Cate_Set_Latest_Image_Index( 0xFFFFFFFF );
	Cate_Set_Latest_Movie_Index( 0xFFFFFFFF );

	for( i = 0; i < obj_count; i++ ) {
		if( FJ_ERR_OK != FJ_GetDCFInfo( dir_obj_v2.word , &dir_key, &file_key, &file_type, ( INT32 * )&attribute, &file_dup_status ) ) {
			break;
		}

		if( FJ_ERR_OK != FJ_GetDCFDBStrVal( FJ_PARA_DCFDB_EXT_GET_STR, file_type, ext_str ) ) {
			break;
		}

		if( strcmp( ext_str, "JPG" ) == 0 ) {
			if( TRUE == BF_Play_Macro_Wrap_Check_Jpeg_Thumbnail( dir_obj_v2.word ) ) {
				if( Cate_Get_Latest_Image_Index() == 0xFFFFFFFF ) {
					Cate_Set_Latest_Image_Index( dir_obj_v2.word );
				}
			}
		}
		else if( strcmp( ext_str, "MP4" ) == 0 ) {
			if( Cate_Get_Latest_Movie_Index() == 0xFFFFFFFF ) {
				Cate_Set_Latest_Movie_Index( dir_obj_v2.word );
			}
		}
		else if( strcmp( ext_str, "MOV" ) == 0 ) {
			if( Cate_Get_Latest_Movie_Index() == 0xFFFFFFFF ) {
				Cate_Set_Latest_Movie_Index( dir_obj_v2.word );
			}
		}

		if( ( Cate_Get_Latest_Image_Index() != 0xFFFFFFFF )
				&& ( Cate_Get_Latest_Movie_Index() != 0xFFFFFFFF ) ) {
			break;
		}

		dir_obj_v2.dir_obj.obj_index -= 1;
	}

	return OK;
}

INT32 User_Utility_Playback_Set_Disp_If( E_SHARE_DISP_IF disp_if )
{
	if( g_playback_video_status != USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	if( ( g_playback_disp_id != E_SHARE_DISP_IF_LCD )
			&& ( g_playback_disp_id != E_SHARE_DISP_IF_HDMI ) ) {
		return NG;
	}

	g_playback_disp_id = disp_if;
	User_Utility_disp_switch( g_playback_disp_id );

	return OK;
}

static INT32 user_utility_playback_get_demux_video_size( UINT32 width, UINT32 lines, E_FJ_MOVIE_PLAY_VIDEO_SIZE * image_size )
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
		return NG;
	}

	return OK;
}

static INT32 user_utility_playback_get_thumbnail_info( UINT16 position_index )
{
	INT32 i;
	THUMBNAIL_CONFIGURATION * thumbnail_config = BF_Play_Attr_Get_Thumbnail_Config();
	USER_UTILITY_PLAYBACK_DIR_OBJ dir_obj = {0};

	INT32							dir_key;				/* DCF directory key			*/
	INT32							file_key;				/* DCF file key					*/
	SHORT							file_type;				/* DCF file type				*/
	UINT32							attribute;				/* attribute ( file/object )	*/
	INT32							file_dup_status;		/* DCF duplicate status			*/
	USHORT							ext_type;				/* File extension type			*/
	CHAR							ext_str[4] = {'\0'};

	g_thumbnail_info.position_index = position_index;
	g_thumbnail_info.line_num = thumbnail_config->bThumbPerColumn;
	g_thumbnail_info.column_num = thumbnail_config->bThumbPerLine;
	g_thumbnail_info.total_page = ( g_thumbnail_info.total_num - 1 ) / ( g_thumbnail_info.line_num * g_thumbnail_info.column_num ) + 1;
	g_thumbnail_info.current_page = g_thumbnail_info.current_num / ( g_thumbnail_info.line_num * g_thumbnail_info.column_num ) + 1;

	if( g_thumbnail_info.current_page == g_thumbnail_info.total_page ) {
		g_thumbnail_info.thumbnail_num = g_thumbnail_info.total_num - g_thumbnail_info.current_num;
	}
	else {
		g_thumbnail_info.thumbnail_num = g_thumbnail_info.line_num * g_thumbnail_info.column_num;
	}

	g_thumbnail_info.position = ( USER_UTILITY_PLAYBACK_THUMBNAIL_POS * )BF_Play_Attr_Get_Thumbnail_Pos( g_playback_disp_id );

	if( g_playback_disp_id == E_SHARE_DISP_IF_LCD ) {
		g_thumbnail_info.width = BF_Play_Attr_Get_Thumb_Draw_Width_Lcd();
		g_thumbnail_info.lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Lcd();
	}
	else {
		g_thumbnail_info.width = BF_Play_Attr_Get_Thumb_Draw_Width_Hdmi();
		g_thumbnail_info.lines = BF_Play_Attr_Get_Thumb_Draw_Lines_Hdmi();
	}

	dir_obj.word = FJ_GetCurrentImage();

	for( i = 0; i < g_thumbnail_info.line_num * g_thumbnail_info.column_num; i++ ) {
		ext_type = 0;

		do {
			g_thumbnail_info.file_list[i].word = dir_obj.word;
			dir_obj.dir_obj.obj_index += 1;

			if( FJ_ERR_OK != FJ_GetDCFInfo( g_thumbnail_info.file_list[i].word, &dir_key, &file_key, &file_type, ( INT32 * )&attribute, &file_dup_status ) ) {
				break;
			}

			if( FJ_ERR_OK != FJ_GetDCFDBStrVal( FJ_PARA_DCFDB_EXT_GET_STR, file_type, ext_str ) ) {
				break;
			}

			if( strcmp( ext_str, "JPG" ) == 0 ) {
				if( TRUE == BF_Play_Macro_Wrap_Check_Jpeg_Thumbnail( g_thumbnail_info.file_list[i].word ) ) {
					ext_type = FJ_FILE_TYPE_JPEG;
				}
			}
			else if( strcmp( ext_str, "MP4" ) == 0 ) {
				ext_type = FJ_FILE_TYPE_MP4;
			}
			else if( strcmp( ext_str, "MOV" ) == 0 ) {
				ext_type = FJ_FILE_TYPE_MOV;
			}
		}
		while( ext_type != FJ_FILE_TYPE_JPEG && ext_type != FJ_FILE_TYPE_MP4 && ext_type != FJ_FILE_TYPE_MOV );
	}

	return OK;
}

static INT32 user_utility_playback_get_thumbnail_config( USER_UTILITY_PLAYBACK_THUMBNAIL_STYLE thumbnail_style, THUMBNAIL_CONFIGURATION * thumbnail_config )
{
	INT32					ret;
	UINT32					disp_width;
	UINT32					disp_line;
	UINT32					thumb_width;
	UINT32					thumb_line;
	THUMBNAIL_CONFIGURATION * thumb_config = BF_Play_Attr_Get_Thumbnail_Config();

	ret = User_Utility_Disp_Get_Resolution_Ratio( g_playback_disp_id, &disp_width, &disp_line );
	UPRINTF_ERR( ret );
	
	if( g_playback_disp_id == E_SHARE_DISP_IF_LCD ) {
		//do nothing.
	}
	else {
		BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Width( disp_width );
		BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Lines( disp_line );
		BF_Play_Attr_Set_Thumb_Width_Hdmi( disp_width );
		BF_Play_Attr_Set_Thumb_Height_Hdmi( disp_line );
	}

	memcpy( thumbnail_config, thumb_config, sizeof( THUMBNAIL_CONFIGURATION ) );

	switch( thumbnail_style ) {
		case USER_UTILITY_PLAYBACK_THUMBNAIL_STYLE_3X3:
			thumbnail_config->bThumbPerLine = 3;
			thumbnail_config->bThumbPerColumn = 3;
			break;

		case USER_UTILITY_PLAYBACK_THUMBNAIL_STYLE_4X4:
			thumbnail_config->bThumbPerLine = 4;
			thumbnail_config->bThumbPerColumn = 4;
			break;

		case USER_UTILITY_PLAYBACK_THUMBNAIL_STYLE_5X5:
			thumbnail_config->bThumbPerLine = 5;
			thumbnail_config->bThumbPerColumn = 5;
			break;

		default:
			return NG;
	}

	//thumb_width = WIDTH_THUMB;
	//thumb_line  = LINES_THUMB;
	Cate_Get_Thumbnail_Size( ( ULONG * )&thumb_width, ( ULONG * )&thumb_line );

	if( thumb_width * thumbnail_config->bThumbPerLine >= disp_width ) {
		thumb_width = disp_width / ( thumbnail_config->bThumbPerLine + 2 );
	}

	if( thumb_line * thumbnail_config->bThumbPerColumn >= disp_line ) {
		thumb_line = disp_line / ( thumbnail_config->bThumbPerColumn + 2 );
	}

	if( thumb_line * 4 / 3 <= thumb_width ) {
		thumb_width = thumb_line * 4 / 3;
	}
	else {
		thumb_line = thumb_width * 3 / 4;
	}

	BF_Play_Attr_Set_Thumb_Draw_Width_Hdmi( thumb_width );
	BF_Play_Attr_Set_Thumb_Draw_Lines_Hdmi( thumb_line );

	if( g_playback_disp_id == E_SHARE_DISP_IF_LCD ) {
		thumbnail_config->bHGap = ( disp_width - ( thumb_width * thumbnail_config->bThumbPerLine ) ) / ( thumbnail_config->bThumbPerLine + 1 );
		thumbnail_config->bVGap = ( disp_line - ( thumb_line * thumbnail_config->bThumbPerColumn ) ) / ( thumbnail_config->bThumbPerColumn + 1 );

		thumbnail_config->bHGap = ( thumbnail_config->bHGap < thumbnail_config->bVGap ) ? thumbnail_config->bHGap : thumbnail_config->bVGap;
		thumbnail_config->bVGap = thumbnail_config->bHGap;

		thumbnail_config->bFirstHGap = ( disp_width - thumb_width * thumbnail_config->bThumbPerLine - thumbnail_config->bHGap * ( thumbnail_config->bThumbPerLine - 1 ) ) / 2;
		thumbnail_config->bLastHGap = 0;

		thumbnail_config->bFirstVGap = ( disp_line - thumb_line * thumbnail_config->bThumbPerColumn - thumbnail_config->bVGap * ( thumbnail_config->bThumbPerColumn - 1 ) ) / 2;
		thumbnail_config->bLastVGap = 0;
	}
	else {
		thumbnail_config->bHGap_HD = ( disp_width - ( thumb_width * thumbnail_config->bThumbPerLine ) ) / ( thumbnail_config->bThumbPerLine + 1 );
		thumbnail_config->bVGap_HD = ( disp_line - ( thumb_line * thumbnail_config->bThumbPerColumn ) ) / ( thumbnail_config->bThumbPerColumn + 1 );

		thumbnail_config->bHGap_HD = ( thumbnail_config->bHGap_HD < thumbnail_config->bVGap_HD ) ? thumbnail_config->bHGap_HD : thumbnail_config->bVGap_HD;
		thumbnail_config->bVGap_HD = thumbnail_config->bHGap_HD;

		thumbnail_config->bFirstHGap_HD = ( disp_width - thumb_width * thumbnail_config->bThumbPerLine - thumbnail_config->bHGap_HD * ( thumbnail_config->bThumbPerLine - 1 ) ) / 2;
		thumbnail_config->bLastHGap_HD = 0;

		thumbnail_config->bFirstVGap_HD = ( disp_line - thumb_line * thumbnail_config->bThumbPerColumn - thumbnail_config->bVGap_HD * ( thumbnail_config->bThumbPerColumn - 1 ) ) / 2;
		thumbnail_config->bLastVGap_HD = 0;
	}

	return OK;
}

static INT32 user_utility_playback_set_file_by_number( ULLONG file_number )
{
	INT32							ret;
	USHORT							dir_count = 0;
	USER_UTILITY_PLAYBACK_DIR_OBJ	dir_obj = {0};
	INT32							dir_key;				/* DCF directory key			*/
	INT32							file_key;				/* DCF file key					*/
	SHORT							file_type;				/* DCF file type				*/
	UINT32							attribute;				/* attribute ( file/object )	*/
	INT32							file_dup_status;		/* DCF duplicate status			*/
	USHORT							ext_type;				/* File extension type			*/
	CHAR							file_name[256] = {'\0'};
	CHAR							ext_str[4] = {'\0'};
	CHAR							file_name_str[5] = {'\0'};
	INT32							i;

	ret = FJ_DirectDcfDirNum( &dir_count );
	UPRINTF_ERR( ret );

	for( i = dir_count; i > 0; i-- ) {
		ret = FJ_SetCurrentDirectory( i );
		UPRINTF_ERR( ret );

		if( ret != FJ_ERR_OK ) {
			continue;
		}

		ret = FJ_Get_Latest_Index( ( INT32 * )&dir_obj );
		UPRINTF_ERR( ret );

		if( ret != FJ_ERR_OK ) {
			continue;
		}

		ret = FJ_GetDCFDBStrVal( FJ_PARA_DCFDB_DIR_GET_STR, dir_obj.word, file_name );
		UPRINTF_ERR( ret );

		Cate_Get_Dcf_File_Name( file_name_str );
		sprintf( file_name + strlen( file_name ), "\\%s%04d", file_name_str, ( UINT32 )file_number );

		ret = FJ_GetDCFDBLongValByStr( FJ_PARA_DCFDB_DIR_OBJ_GET_BY_STR, file_name, ( LONG * )&dir_obj.word );
		UPRINTF_ERR( ret );

		if( ret != FJ_ERR_OK ) {
			continue;
		}

		ret = FJ_SetCurrentImage( dir_obj.word );
		UPRINTF_ERR( ret );

		if( ret != FJ_ERR_OK ) {
			continue;
		}

		break;
	}

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	ret = FJ_GetDCFDBStrVal( FJ_PARA_DCFDB_FULL_PATH_GET_STR, dir_obj.word, file_name );
	UPRINTF_ERR( ret );

	strncpy( g_playback_file_name, file_name, sizeof( file_name ) );

	ret = FJ_GetDCFInfo( dir_obj.word, &dir_key, &file_key, &file_type, ( INT32 * )&attribute, &file_dup_status );
	UPRINTF_ERR( ret );

	ret = FJ_GetDCFDBStrVal( FJ_PARA_DCFDB_EXT_GET_STR, file_type, ext_str );
	UPRINTF_ERR( ret );

	if( strcmp( ext_str, "JPG" ) == 0 ) {
		ext_type = FJ_FILE_TYPE_JPEG;
	}
	else if( strcmp( ext_str, "MP4" ) == 0 ) {
		ext_type = FJ_FILE_TYPE_MP4;
	}
	else if( strcmp( ext_str, "MOV" ) == 0 ) {
		ext_type = FJ_FILE_TYPE_MOV;
	}
	else {
		return NG;
	}

	ret = FJ_SetFileType( ext_type );
	UPRINTF_ERR( ret );

	g_playback_file_type = ext_type;

	BF_Play_Attr_Set_Image_Index( dir_obj.word );
	BF_Play_Attr_Set_File_Type( ext_type );

	return OK;
}

static INT32 user_utility_playback_set_file_by_index( UINT32 file_index )
{
	INT32							ret;
	USER_UTILITY_PLAYBACK_DIR_OBJ	dir_obj = {0};
	INT32							dir_key;				/* DCF directory key			*/
	INT32							file_key;				/* DCF file key					*/
	SHORT							file_type;				/* DCF file type				*/
	UINT32							attribute;				/* attribute ( file/object )	*/
	INT32							file_dup_status;		/* DCF duplicate status			*/
	USHORT							ext_type;				/* File extension type			*/
	CHAR							file_name[256] = {'\0'};
	CHAR							ext_str[4] = {'\0'};

	dir_obj.word = file_index;

	ret = FJ_SetCurrentDirectory( dir_obj.dir_obj.dir_index );
	UPRINTF_ERR( ret );

	ret = FJ_SetCurrentImage( dir_obj.word );
	UPRINTF_ERR( ret );

	ret = FJ_GetDCFDBStrVal( FJ_PARA_DCFDB_FULL_PATH_GET_STR, dir_obj.word, file_name );
	UPRINTF_ERR( ret );

	strncpy( g_playback_file_name, file_name, sizeof( file_name ) );

	ret = FJ_GetDCFInfo( dir_obj.word, &dir_key, &file_key, &file_type, ( INT32 * )&attribute, &file_dup_status );
	UPRINTF_ERR( ret );

	ret = FJ_GetDCFDBStrVal( FJ_PARA_DCFDB_EXT_GET_STR, file_type, ext_str );
	UPRINTF_ERR( ret );

	if( strcmp( ext_str, "JPG" ) == 0 ) {
		ext_type = FJ_FILE_TYPE_JPEG;
	}
	else if( strcmp( ext_str, "MP4" ) == 0 ) {
		ext_type = FJ_FILE_TYPE_MP4;
	}
	else if( strcmp( ext_str, "MOV" ) == 0 ) {
		ext_type = FJ_FILE_TYPE_MOV;
	}
	else {
		return NG;
	}

	ret = FJ_SetFileType( ext_type );
	UPRINTF_ERR( ret );

	g_playback_file_type = ext_type;
	return OK;
}

static INT32 user_utility_playback_jpeg_start( VOID )
{
	INT32 ret;

	if( g_playback_file_type == FJ_FILE_TYPE_JPEG ) {
		ret = user_utility_playback_jpeg_display( g_playback_file_name, FJ_DISP_ROTATE_DEGREE_0 );
		UPRINTF_ERR( ret );
	}
	else if( ( g_playback_file_type == FJ_FILE_TYPE_MP4 )
			|| ( g_playback_file_type == FJ_FILE_TYPE_MOV ) ) {
		ret = user_utility_playback_video_display( 0, FJ_DISP_ROTATE_DEGREE_0 );
		UPRINTF_ERR( ret );

		ret = user_utility_playback_video_end();
		UPRINTF_ERR( ret );
	}

	return OK;
}

static INT32 user_utility_playback_jpeg_display( CHAR * file_name, UCHAR rotate )
{
	INT32					ret;
	INT32					file_index;
	FJ_FS_STAT				file_stat;
	T_FJ_PLAYBACK_DECODE	dec_info;
	T_FJ_PLAYBACK_YUV_INFO	yuv_info;
	T_FJ_PLAYBACK_CONVERT	cnv_info;
	FJ_DISP_YCC_ADDR		disp_ycc_addr;
	UINT32					offset_cb, offset_cr;
	UINT32					src_addr, dst_addr;
	USHORT					g_width;
	USHORT					jpeg_width;
	USHORT					jpeg_line;
	USHORT					data_len;
	IMAGE_ATTRIBUTES		image_attr;

	memset( &dec_info, 0, sizeof( dec_info ) );
	memset( &yuv_info, 0, sizeof( yuv_info ) );
	memset( &cnv_info, 0, sizeof( cnv_info ) );

	ret = FJ_Fs_Stat( file_name, &file_stat );
	UPRINTF_ERR( ret );

	file_index = FJ_GetCurrentImage();

	ret = FJ_GetImageEXIF( file_index, FJ_IFD_0, FJ_IFD0_TAG_XRES_, ( BYTE * )&jpeg_width, &data_len );
	UPRINTF_ERR( ret );

	ret = FJ_GetImageEXIF( file_index, FJ_IFD_0, FJ_IFD0_TAG_YRES_, ( BYTE * )&jpeg_line, &data_len );
	UPRINTF_ERR( ret );

	ret = FJ_GetImageAttributes( &image_attr );
	UPRINTF_ERR( ret );

	g_jpeg_info.offset	= 0;
	g_jpeg_info.size	= file_stat.size;
	g_jpeg_info.width	= jpeg_width;
	g_jpeg_info.lines	= jpeg_line;

	if( image_attr.wFlags.sampling ) {
		g_jpeg_info.format	= FJ_IMG_PFMT_YCC422;
	}
	else {
		g_jpeg_info.format	= FJ_IMG_PFMT_YCC420;
	}
	
	ret = user_utility_playback_jpeg_load( ( char * )file_name );
	UPRINTF_ERR( ret );

	if( ret ) {
		return NG;
	}

	memset( &g_mem_info, 0, sizeof( g_mem_info ) );
	ret = FJ_StillPlayback_GetMemInfo( &g_mem_info );
	UPRINTF_ERR( ret );

	dec_info.src_addr	= g_jpeg_addr;
	dec_info.src_size	= g_jpeg_info.size;
	dec_info.src_width	= g_jpeg_info.width;
	dec_info.src_lines	= g_jpeg_info.lines;
	dec_info.dst_addr = g_mem_info.decoded_output_yuv_main.top_addr;
	
	switch( g_jpeg_info.format ) {
		case 0:
			dec_info.dst_fmt = E_FJ_PLAYBACK_FMT_YCC422;
			break;

		case 1:
			dec_info.dst_fmt = E_FJ_PLAYBACK_FMT_YCC420;
			break;

		default:
			return NG;
	}

	ret = FJ_StillPlayback_ImageDecode( dec_info, &yuv_info );
	UPRINTF_ERR( ret );

	g_jpeg_info.g_width = yuv_info.g_width;
	g_jpeg_info.g_lines = yuv_info.g_lines;
	src_addr = g_mem_info.decoded_output_yuv_main.top_addr;

	if( g_playback_disp_id == E_SHARE_DISP_IF_LCD ) {
		dst_addr = g_mem_info.disp_yuv_lcd.top_addr;
	}
	else {
		dst_addr = g_mem_info.disp_yuv_hdmi.top_addr;
	}

	switch( g_jpeg_info.format ) {
		case 0:
			offset_cb = g_jpeg_info.g_width * g_jpeg_info.g_lines;
			offset_cr = offset_cb / 2;
			cnv_info.src_img.pfmt	= FJ_IMG_PFMT_YCC422;
			break;

		case 1:
			offset_cb = g_jpeg_info.g_width * g_jpeg_info.g_lines;
			offset_cr = offset_cb / 4;
			cnv_info.src_img.pfmt	= FJ_IMG_PFMT_YCC420;
			break;

		default:
			return NG;
	}

	cnv_info.src_img.addr.AddressY		= src_addr;
	cnv_info.src_img.addr.AddressCb		= ( UINT32 )( cnv_info.src_img.addr.AddressY + offset_cb );
	cnv_info.src_img.addr.AddressCr		= ( UINT32 )( cnv_info.src_img.addr.AddressCb + offset_cr );
	cnv_info.src_img.g_y_width			= g_jpeg_info.g_width;
	cnv_info.src_img.g_c_width			= cnv_info.src_img.g_y_width;
	cnv_info.src_img.width				= yuv_info.width;
	cnv_info.src_img.lines				= yuv_info.lines;

	offset_cb = jpeg_width * jpeg_line;
	offset_cr = offset_cb / 2;

	cnv_info.dst_img.addr.AddressY		= dst_addr;
	cnv_info.dst_img.addr.AddressCb		= ( UINT32 )( cnv_info.dst_img.addr.AddressY + offset_cb );
	cnv_info.dst_img.addr.AddressCr		= ( UINT32 )( cnv_info.dst_img.addr.AddressCb + offset_cr );
	cnv_info.dst_img.pfmt				= FJ_IMG_PFMT_YCC420;
	cnv_info.dst_img.g_y_width			= jpeg_width;
	cnv_info.dst_img.g_c_width			= cnv_info.dst_img.g_y_width;
	cnv_info.dst_img.width				= jpeg_width;
	cnv_info.dst_img.lines				= jpeg_line;

	cnv_info.resize_mode				= FJ_IMG_RESIZE_MODE_BILINEAR;
	cnv_info.rotate_deg					= ( FJ_IMG_ROTATE_DEGREE )rotate;

	ret = FJ_StillPlayback_ImageConvert( cnv_info );
	UPRINTF_ERR( ret );

	if( g_playback_disp_id == E_SHARE_DISP_IF_LCD ) {
		disp_ycc_addr.y_addr = ( UINT32 )g_mem_info.disp_yuv_lcd.top_addr;
	}
	else {
		disp_ycc_addr.y_addr = ( UINT32 )g_mem_info.disp_yuv_hdmi.top_addr;
	}

	disp_ycc_addr.cbcr_addr = disp_ycc_addr.y_addr + ( cnv_info.dst_img.g_y_width * cnv_info.dst_img.lines );

	if( ( rotate != 0 ) && ( rotate != 2 ) ) {
		g_width = cnv_info.dst_img.lines;
	}
	else {
		g_width = cnv_info.dst_img.g_y_width;
	}

	ret = User_Utility_Disp_Disable( g_playback_disp_id );
	UPRINTF_ERR( ret );

	ret = User_Utility_Disp_Set_Window( g_playback_disp_id, jpeg_width, jpeg_line, FJ_DISP_ROTATE_DEGREE_0 );
	UPRINTF_ERR( ret );

	ret = User_Utility_Disp_Render( g_playback_disp_id, disp_ycc_addr, g_width );
	UPRINTF_ERR( ret );

	ret = User_Utility_Disp_Enable( g_playback_disp_id );
	UPRINTF_ERR( ret );

	return OK;
}

static INT32 user_utility_playback_jpeg_load( CHAR * file_path )
{
	INT32		ret;
	UINT32		addr, size = 1;
	ULONG		file_id = 0;
 	ULONG		read_size = 0;
	INT64		value;
	FJ_FS_STAT	stat;

	ret = FJ_GetMem( FJ_HOST_JPEG_DEMUXER_READ_MEM_LOCATION, &addr, &size );
	UPRINTF_ERR( ret );

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	g_jpeg_addr = addr;
	file_id = FJ_Fs_Open( file_path, FJ_FS_OPEN_RDWR );

	if( file_id == 0 ) {
		return NG;
	}

	g_playback_file_id = file_id;
	ret = FJ_Fs_Lseek64( ( ULONG )g_playback_file_id, ( INT64 )g_jpeg_info.offset, ( USHORT )FJ_FS_SEEK_SET, ( INT64 * )&value );
	UPRINTF_ERR( ret );

	if( ret != FJ_ERR_OK ) {
		FJ_Fs_Close( g_playback_file_id );
		return NG;
	}

	ret = FJ_Fs_Stat( file_path, &stat );
	UPRINTF_ERR( ret );

	if( ret != FJ_ERR_OK ) {
		FJ_Fs_Close( g_playback_file_id );
		return NG;
	}

	g_jpeg_info.size = ( UINT32 )stat.size;

	if( g_jpeg_info.size == 0 ) {
		FJ_Fs_Close( g_playback_file_id );
		return NG;
	}

	ret = FJ_Fs_Read( g_playback_file_id, ( UINT32 * )g_jpeg_addr, ( ULONG )g_jpeg_info.size, &read_size );
	UPRINTF_ERR( ret );

	if( ret == NG ) {
		FJ_Fs_Close( g_playback_file_id );
		return NG;
	}

	FJ_Fs_Close( g_playback_file_id );
	return OK;
}

static INT32 user_utility_playback_video_display( UINT32 start_pts, FJ_DISP_ROTATE_DEGREE rotate )
{
	INT32											ret;
	UINT32											file_id = 0;
	UINT32											header_addr = 0;
	UINT32											header_size = 1;
	T_FJ_MUX_DEMUX_COMMON_MEMORY_CONFIG				mem_config;
	T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM				video_config;
	UINT32											read_stream_num;
	T_FJ_DEMUX_STREAM_PARAM							demux_stream_param;
	T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM				video_stream_param[D_USER_UTILITY_PLAYBACK_VIDEO_STREAM_MAX];
	T_FJ_MOVIE_PLAY_VIDEO_YUV_INFO					decode_yuv_info;
	T_FJ_PLAYBACK_CONVERT							cnv_info;
	FJ_DISP_YCC_ADDR								ycc_addr;
	UINT32											ycc_g_y_width = 0;
	UINT32											g_y_width;
	INT32											i;

	memset( &g_mem_info, 0, sizeof( g_mem_info ) );
	ret = FJ_StillPlayback_GetMemInfo( &g_mem_info );
	UPRINTF_ERR( ret );

	file_id = FJ_Fs_Open( g_playback_file_name, FJ_FS_OPEN_RDWR );

	if( file_id == 0 ) {
		return NG;
	}

	g_playback_file_id = file_id;

	ret = FJ_GetMem( FJ_HOST_MP4_DEMUXER_PARSED_MEM_LOCATION, &header_addr, &header_size );
	UPRINTF_ERR( ret );

	mem_config.header_mem_addr	= header_addr;
	mem_config.header_mem_size	= header_size;
	mem_config.muxing_mem_addr	= 0;
	mem_config.muxing_mem_size	= 0;
	mem_config.muxing_file_num	= 0;
	FJ_Mux_Demux_Common_Memory_Initialize( &mem_config );

	ret = FJ_Demux_Initialize( g_playback_file_id, ( T_FJ_DEMUX_HEADER_INFO * )&g_demux_header_info );
	UPRINTF_ERR( ret );

	if( start_pts % g_demux_header_info.video_time_delta ) {
		return NG;
	}

	if( start_pts > g_demux_header_info.video_play_last_pts ) {
		start_pts = g_demux_header_info.video_play_last_pts;
	}

	g_demux_pts = start_pts;

	if( ( rotate == FJ_DISP_ROTATE_DEGREE_90 ) || ( rotate == FJ_DISP_ROTATE_DEGREE_270 ) ) {
		rotate = FJ_DISP_ROTATE_DEGREE_0;
	}

	ret = FJ_Demux_VideoSetup( g_playback_file_id, g_demux_pts, E_FJ_DEMUX_DIRECTION_FWD, E_FJ_DEMUX_SPEED_1X );
	UPRINTF_ERR( ret );

	if( g_demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H264 ) {
		ret = FJ_VideoDecoder_Open( E_FJ_VIDEO_DEC_H264, &g_video_dec_id );
		UPRINTF_ERR( ret );
	}
	else if( g_demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H265 ) {
		ret = FJ_VideoDecoder_Open( E_FJ_VIDEO_DEC_H265, &g_video_dec_id );
		UPRINTF_ERR( ret );
	}
	else {
		return NG;
	}

	ret = FJ_VideoDecoder_Connect( g_video_dec_id, g_playback_disp_id, FALSE );
	UPRINTF_ERR( ret );

	ret = FJ_VideoPlayback_VideoInit();
	UPRINTF_ERR( ret );

	ret = User_Utility_Disp_Disable( g_playback_disp_id );
	UPRINTF_ERR( ret );

	ret = User_Utility_Disp_Set_Window( g_playback_disp_id, g_demux_header_info.stream_width, g_demux_header_info.stream_height, rotate );
	UPRINTF_ERR( ret );

	if( g_demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H264 ) {
		video_config.codec_type		= FJ_MOVIE_PLAY_CODEC_H264;
	}
	else if( g_demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H265 ) {
		video_config.codec_type		= FJ_MOVIE_PLAY_CODEC_H265;
	}
	else {
		return NG;
	}

	ret = user_utility_playback_get_demux_video_size( g_demux_header_info.stream_width, g_demux_header_info.stream_height, &video_config.image_size );
	UPRINTF_ERR( ret );

	video_config.scan_mode			= FJ_MOVIE_PLAY_VIDEO_SCAN_MODE_PROGRESSIVE;
	video_config.conceal_error		= FJ_MOVIE_PLAY_VIDEO_CONCEAL_ERROR_SKIP;
	video_config.time_scale			= g_demux_header_info.video_time_scale;
	video_config.time_delta			= g_demux_header_info.video_time_delta;
	video_config.trick				= TRUE;
	video_config.compatibility_mode	= FJ_MOVIE_PLAY_OFF;
	ret = FJ_VideoPlayback_SetVideoConfig( &video_config );
	UPRINTF_ERR( ret );

	if( g_demux_pts == 0 ) {
		read_stream_num = g_demux_header_info.offset_of_start_frame + 1;

		if( g_demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H265 ) {
			read_stream_num++;
		}
	}
	else {
		read_stream_num = ( g_demux_header_info.video_time_scale / g_demux_header_info.video_time_delta ) + 1;
		read_stream_num /= 2;
	}

	ret = FJ_Demux_StreamReadSync( g_playback_file_id, g_demux_header_info.video_codec, read_stream_num, &demux_stream_param );
	UPRINTF_ERR( ret );

	for( i = 0; i < demux_stream_param.stream_num; i++ ) {
		video_stream_param[i].stream_end_flag 		= demux_stream_param.stream_end_flag[i];
		video_stream_param[i].stss					= demux_stream_param.stss[i];
		video_stream_param[i].stream_size 			= demux_stream_param.stream_size[i];
		video_stream_param[i].stream_address		= demux_stream_param.stream_address[i];
		video_stream_param[i].pts 					= demux_stream_param.pts[i];
		video_stream_param[i].dts 					= demux_stream_param.dts[i];
	}

	ret = FJ_VideoPlayback_StartIFrameDecoding( !g_playback_disp_id, g_demux_pts, read_stream_num, video_stream_param, &decode_yuv_info );
	UPRINTF_ERR( ret );

	cnv_info.src_img.pfmt				= FJ_IMG_PFMT_VIDEO;
	cnv_info.src_img.addr.AddressY		= decode_yuv_info.y_addr;
	cnv_info.src_img.addr.AddressCb		= decode_yuv_info.cbcr_addr;
	cnv_info.src_img.addr.AddressCr		= decode_yuv_info.cbcr_addr;
	cnv_info.src_img.g_y_width			= decode_yuv_info.g_width;
	cnv_info.src_img.g_c_width			= decode_yuv_info.g_width;
	cnv_info.src_img.width				= decode_yuv_info.width;
	cnv_info.src_img.lines				= decode_yuv_info.lines;
	cnv_info.dst_img.pfmt				= FJ_IMG_PFMT_YCC420;

	if( 0 != ( g_demux_header_info.stream_width % 8 ) ) {
		g_y_width = ( ( g_demux_header_info.stream_width / 8 ) + 1 ) * 8;
	}
	else {
		g_y_width = g_demux_header_info.stream_width;
	}

	if( g_playback_disp_id == E_SHARE_DISP_IF_LCD ) {
		cnv_info.dst_img.addr.AddressY		= g_mem_info.disp_yuv_lcd.top_addr;
	}
	else {
		cnv_info.dst_img.addr.AddressY		= g_mem_info.disp_yuv_hdmi.top_addr;
	}

	cnv_info.dst_img.addr.AddressCb		= cnv_info.dst_img.addr.AddressY + g_y_width * g_demux_header_info.stream_height;
	cnv_info.dst_img.addr.AddressCr		= cnv_info.dst_img.addr.AddressY + g_y_width * g_demux_header_info.stream_height;
	cnv_info.dst_img.g_y_width			= g_y_width;
	cnv_info.dst_img.g_c_width			= g_y_width;
	cnv_info.dst_img.width				= g_demux_header_info.stream_width;
	cnv_info.dst_img.lines				= g_demux_header_info.stream_height;
	cnv_info.resize_mode				= FJ_IMG_RESIZE_MODE_BILINEAR;

	if( rotate == FJ_DISP_ROTATE_DEGREE_0 ) {
		cnv_info.rotate_deg = FJ_IMG_ROTATE_DEGREE_0;
	}
	else if( rotate == FJ_DISP_ROTATE_DEGREE_90 ) {
		cnv_info.rotate_deg = FJ_IMG_ROTATE_DEGREE_90;
	}
	else if( rotate == FJ_DISP_ROTATE_DEGREE_180 ) {
		cnv_info.rotate_deg = FJ_IMG_ROTATE_DEGREE_180;
	}
	else if( rotate == FJ_DISP_ROTATE_DEGREE_270 ) {
		cnv_info.rotate_deg = FJ_IMG_ROTATE_DEGREE_270;
	}
	else {
		cnv_info.rotate_deg = FJ_IMG_ROTATE_DEGREE_0;
	}

	ret = FJ_StillPlayback_ImageConvert( cnv_info );
	UPRINTF_ERR( ret );

	ycc_addr.y_addr		= cnv_info.dst_img.addr.AddressY;
	ycc_addr.cbcr_addr	= cnv_info.dst_img.addr.AddressCb;
	ycc_g_y_width		= cnv_info.dst_img.g_y_width;

	ret = User_Utility_Disp_Render( g_playback_disp_id, ycc_addr, ycc_g_y_width );
	UPRINTF_ERR( ret );

	ret = User_Utility_Disp_Enable( g_playback_disp_id );
	UPRINTF_ERR( ret );

	return OK;
}

static INT32 user_utility_playback_video_start( UINT32 start_pts, E_FJ_MOVIE_PLAY_DIRECTION direction, E_FJ_MOVIE_PLAY_SPEED speed, FJ_DISP_ROTATE_DEGREE rotate )
{
	INT32											ret;
	UINT32											file_id = 0;
	INT64											value;
	UINT32											header_addr = 0;
	UINT32											header_size = 1;
	T_FJ_MUX_DEMUX_COMMON_MEMORY_CONFIG				mem_config;
	T_FJ_MOVIE_PLAY_VIDEO_CONFIG_PARAM				video_config;
	E_FJ_DEMUX_DIRECTION							demux_direction;
	E_FJ_DEMUX_SPEED								demux_speed;
	UINT32											read_stream_num;
	UINT32											video_frame_rate_multiply_100;
	T_FJ_DEMUX_STREAM_PARAM							demux_stream_param;
	T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM				video_stream_param[D_USER_UTILITY_PLAYBACK_VIDEO_STREAM_MAX];
	T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM				audio_stream_param[D_USER_UTILITY_PLAYBACK_AUDIO_STREAM_MAX];
	UINT32											audio_start_pts = 0;
	T_FJ_AUDIO_DEC_CONFIG_PARAM						audio_dec_config;
	T_FJ_AUDIO_OUT_CFG								audio_out_config;
	INT32											i;

	file_id = FJ_Fs_Open( g_playback_file_name, FJ_FS_OPEN_RDWR );

	if( file_id == 0 )
	{
		return NG;
	}

	g_playback_file_id = file_id;

	ret = FJ_Fs_Lseek64( ( ULONG )g_playback_file_id, ( INT64 )0, ( USHORT )FJ_FS_SEEK_SET, ( INT64 * )&value );
	UPRINTF_ERR( ret );

	ret = FJ_GetMem( FJ_HOST_MP4_DEMUXER_PARSED_MEM_LOCATION, &header_addr, &header_size );
	UPRINTF_ERR( ret );

	mem_config.header_mem_addr	= header_addr;
	mem_config.header_mem_size	= header_size;
	mem_config.muxing_mem_addr	= 0;
	mem_config.muxing_mem_size	= 0;
	mem_config.muxing_file_num	= 0;
	FJ_Mux_Demux_Common_Memory_Initialize( &mem_config );

	ret = FJ_Demux_Initialize( g_playback_file_id, ( T_FJ_DEMUX_HEADER_INFO * )&g_demux_header_info );
	UPRINTF_ERR( ret );

	if( start_pts % g_demux_header_info.video_time_delta ) {
		return NG;
	}

	switch( direction ) {
		case E_FJ_MOVIE_PLAY_DIRECTION_FWD:
			demux_direction = E_FJ_DEMUX_DIRECTION_FWD;
			break;

		case E_FJ_MOVIE_PLAY_DIRECTION_RWD:
			demux_direction = E_FJ_DEMUX_DIRECTION_RWD;
			break;

		default:
			demux_direction = E_FJ_DEMUX_DIRECTION_FWD;
			break;
	}

	if( direction == E_FJ_MOVIE_PLAY_DIRECTION_FWD ) {
		if( ( g_demux_header_info.video_play_last_pts < start_pts ) && ( speed == E_FJ_MOVIE_PLAY_SPEED_1X ) ) {
			start_pts = g_demux_header_info.video_play_last_pts;
		}
		else if( ( g_demux_header_info.video_ff_last_pts < start_pts ) && ( ( speed == E_FJ_MOVIE_PLAY_SPEED_2X ) || ( speed == E_FJ_MOVIE_PLAY_SPEED_4X ) || ( speed == E_FJ_MOVIE_PLAY_SPEED_8X ) ) ) {
			start_pts = g_demux_header_info.video_ff_last_pts - g_demux_header_info.video_time_delta;
		}
	}

	g_demux_pts = start_pts;

	switch( speed ) {
		case E_FJ_MOVIE_PLAY_SPEED_1X:
			demux_speed = E_FJ_DEMUX_SPEED_1X;
			break;

		case E_FJ_MOVIE_PLAY_SPEED_2X:
			demux_speed = E_FJ_DEMUX_SPEED_2X;
			break;

		case E_FJ_MOVIE_PLAY_SPEED_4X:
			demux_speed = E_FJ_DEMUX_SPEED_4X;
			break;

		case E_FJ_MOVIE_PLAY_SPEED_8X:
			demux_speed = E_FJ_DEMUX_SPEED_8X;
			break;

		case E_FJ_MOVIE_PLAY_SPEED_1_2X:
			demux_speed = E_FJ_DEMUX_SPEED_1_2X;
			break;

		case E_FJ_MOVIE_PLAY_SPEED_1_4X:
			demux_speed = E_FJ_DEMUX_SPEED_1_4X;
			break;

		case E_FJ_MOVIE_PLAY_SPEED_1_8X:
			demux_speed = E_FJ_DEMUX_SPEED_1_8X;
			break;

		default:
			demux_speed = E_FJ_DEMUX_SPEED_1X;
			break;
	}

	if( ( rotate == FJ_DISP_ROTATE_DEGREE_90 ) || ( rotate == FJ_DISP_ROTATE_DEGREE_270 ) ) {
		rotate = FJ_DISP_ROTATE_DEGREE_0;
	}

	ret = FJ_Demux_VideoSetup( g_playback_file_id, g_demux_pts, demux_direction, demux_speed );
	UPRINTF_ERR( ret );

	if( g_demux_header_info.audio_codec == E_FJ_DEMUX_CODEC_INVALID ) {
		g_playback_audio_status = USER_UTILITY_PLAYBACK_AUDIO_INVALID;
	}

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		audio_start_pts = ( ( UINT64 )start_pts * g_demux_header_info.audio_time_scale ) / g_demux_header_info.video_time_scale;

		ret = FJ_Demux_AudioSetup( g_playback_file_id, audio_start_pts );
		UPRINTF_ERR( ret );
	}

	if( g_demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H264 ) {
		ret = FJ_VideoDecoder_Open( E_FJ_VIDEO_DEC_H264, &g_video_dec_id );
		UPRINTF_ERR( ret );
	}
	else if( g_demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H265 ) {
		ret = FJ_VideoDecoder_Open( E_FJ_VIDEO_DEC_H265, &g_video_dec_id );
		UPRINTF_ERR( ret );
	}
	else {
		return NG;
	}

	ret = FJ_VideoDecoder_Connect( g_video_dec_id, g_playback_disp_id, FALSE );
	UPRINTF_ERR( ret );

	ret = FJ_VideoPlayback_VideoInit();
	UPRINTF_ERR( ret );

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		ret = FJ_AudioDecoder_Open( E_FJ_AUDIO_DEC_AAC, &g_audio_dec_id );
		UPRINTF_ERR( ret );

		if( g_playback_disp_id == E_SHARE_DISP_IF_LCD ) {
			ret = FJ_AudioOut_Open( E_FJ_AUDIO_OUT_0, &g_audio_out_id );
			UPRINTF_ERR( ret );
		}
		else {
			ret = FJ_AudioOut_Open( E_FJ_AUDIO_OUT_1, &g_audio_out_id );
			UPRINTF_ERR( ret );
		}

		ret = FJ_AudioDecoder_Connect( g_audio_dec_id, g_audio_out_id );
		UPRINTF_ERR( ret );

		ret = FJ_VideoPlayback_AudioInit();
		UPRINTF_ERR( ret );
	}

	ret = User_Utility_Disp_Disable( g_playback_disp_id );
	UPRINTF_ERR( ret );

	ret = User_Utility_Disp_Set_Window( g_playback_disp_id, g_demux_header_info.stream_width, g_demux_header_info.stream_height, rotate );
	UPRINTF_ERR( ret );

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		if( g_playback_disp_id == E_SHARE_DISP_IF_LCD ) {
			audio_out_config.audio_out_port = E_FJ_AUDIO_OUT_PORT_I2S0;
			audio_out_config.i2s_cfg.master_slave = FJ_AUDIO_IF_MASTER;
			audio_out_config.i2s_cfg.lr_clock = g_demux_header_info.sampling_rate;
			audio_out_config.i2s_cfg.data_cycle = 64;
			audio_out_config.i2s_cfg.i2s_format = E_FJ_AUDIO_I2S_Data_FORMAT_LEFT;
			audio_out_config.i2s_cfg.bit_count = 24;
			ret = FJ_AudioOut_Config( g_audio_out_id, &audio_out_config );
			UPRINTF_ERR( ret );

			AudioCodecWrapper_PowerUp();
			AudioCodecWrapper_Play_Start();
		}
		else {
			audio_out_config.audio_out_port = E_FJ_AUDIO_OUT_PORT_I2S3;
			audio_out_config.i2s_cfg.master_slave = FJ_AUDIO_IF_MASTER;
			audio_out_config.i2s_cfg.lr_clock = g_demux_header_info.sampling_rate;
			audio_out_config.i2s_cfg.data_cycle = 64;
			audio_out_config.i2s_cfg.i2s_format = E_FJ_AUDIO_I2S_Data_FORMAT_LEFT;
			audio_out_config.i2s_cfg.bit_count = 24;
			ret = FJ_AudioOut_Config( g_audio_out_id, &audio_out_config );
			UPRINTF_ERR( ret );
		}
	}

	if( g_demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H264 ) {
		video_config.codec_type		= FJ_MOVIE_PLAY_CODEC_H264;
	}
	else if( g_demux_header_info.video_codec == E_FJ_DEMUX_CODEC_VIDEO_H265 ) {
		video_config.codec_type		= FJ_MOVIE_PLAY_CODEC_H265;
	}
	else {
		return NG;
	}

	ret = user_utility_playback_get_demux_video_size( g_demux_header_info.stream_width, g_demux_header_info.stream_height, &video_config.image_size );
	UPRINTF_ERR( ret );

	video_config.scan_mode			= FJ_MOVIE_PLAY_VIDEO_SCAN_MODE_PROGRESSIVE;
	video_config.conceal_error		= FJ_MOVIE_PLAY_VIDEO_CONCEAL_ERROR_SKIP;
	video_config.time_scale			= g_demux_header_info.video_time_scale;
	video_config.time_delta			= g_demux_header_info.video_time_delta;
	video_config.trick				= TRUE;
	video_config.compatibility_mode	= FJ_MOVIE_PLAY_OFF;
	ret = FJ_VideoPlayback_SetVideoConfig( &video_config );
	UPRINTF_ERR( ret );

	if( g_demux_header_info.audio_codec == E_FJ_DEMUX_CODEC_AUDIO_AAC ) {
		audio_dec_config.codec_type = E_FJ_AUDIO_DEC_CODEC_AAC;
	}
	else {
		audio_dec_config.codec_type = E_FJ_AUDIO_DEC_CODEC_PCM;
	}

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		audio_dec_config.audio_ch = g_demux_header_info.audio_ch;
		audio_dec_config.sampling_rate = g_demux_header_info.sampling_rate;
		ret = FJ_AudioDecoder_Config( g_audio_dec_id, &audio_dec_config );
		UPRINTF_ERR( ret );
	}

	ret = FJ_VideoPlayback_SetStartPTS( g_demux_pts );
	UPRINTF_ERR( ret );

	video_frame_rate_multiply_100 = ( g_demux_header_info.video_time_scale * 100 ) / g_demux_header_info.video_time_delta;

	if( video_frame_rate_multiply_100 % 100 ) {
		read_stream_num = ( ( video_frame_rate_multiply_100 / 100 ) + 1 ) / 2;
	}
	else {
		read_stream_num = ( video_frame_rate_multiply_100 / 100 ) / 2;
	}

	if( read_stream_num == 0 ) {
		read_stream_num = 1;
	}

	ret = FJ_Demux_StreamReadSync( g_playback_file_id, g_demux_header_info.video_codec, read_stream_num, &demux_stream_param );
	UPRINTF_ERR( ret );

	for( i = 0; i < demux_stream_param.stream_num; i++ ) {
		video_stream_param[i].stream_end_flag 		= demux_stream_param.stream_end_flag[i];
		video_stream_param[i].stss					= demux_stream_param.stss[i];
		video_stream_param[i].stream_size 			= demux_stream_param.stream_size[i];
		video_stream_param[i].stream_address		= demux_stream_param.stream_address[i];
		video_stream_param[i].pts 					= demux_stream_param.pts[i];
		video_stream_param[i].dts 					= demux_stream_param.dts[i];
	}

	ret = FJ_VideoPlayback_SetVideoStream( video_config.codec_type, demux_stream_param.stream_num, video_stream_param );
	UPRINTF_ERR( ret );

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		ret = FJ_Demux_StreamReadSync( g_playback_file_id, g_demux_header_info.audio_codec, D_USER_UTILITY_PLAYBACK_AUDIO_STREAM_MAX, &demux_stream_param );
		UPRINTF_ERR( ret );

		for( i = 0; i < demux_stream_param.stream_num; i++ ) {
			audio_stream_param[i].stream_end_flag	= demux_stream_param.stream_end_flag[i];
			audio_stream_param[i].stream_size		= demux_stream_param.stream_size[i];
			audio_stream_param[i].stream_address	= demux_stream_param.stream_address[i];
			audio_stream_param[i].pts				= ( ( UINT64 )demux_stream_param.pts[i] * g_demux_header_info.video_time_scale ) / g_demux_header_info.audio_time_scale;
		}

		ret = FJ_VideoPlayback_SetAudioStream( demux_stream_param.stream_num, audio_stream_param );
		UPRINTF_ERR( ret );
	}

	ret = FJ_VideoPlayback_SetSpeedDirection( speed, direction, FALSE );
	UPRINTF_ERR( ret );

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		ret = FJ_VideoPlayback_StartAudioDecode();
		UPRINTF_ERR( ret );
	}

	ret = FJ_VideoPlayback_StartVideoDecode(video_config.codec_type);
	UPRINTF_ERR( ret );

	g_playback_video_status = USER_UTILITY_PLAYBACK_VIDEO_PLAY;

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		g_playback_audio_status = USER_UTILITY_PLAYBACK_AUDIO_ON;
	}

	g_video_end_flag = FALSE;
	g_audio_end_flag = FALSE;
	g_video_error_flag = FALSE;

	return OK;
}

static INT32 user_utility_playback_video_restart( VOID )
{
	INT32											ret;
	T_FJ_DEMUX_STREAM_PARAM							demux_stream_param;
	T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM				audio_stream_param[D_USER_UTILITY_PLAYBACK_AUDIO_STREAM_MAX];
	UINT32											audio_start_pts;
	INT32											i;

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		audio_start_pts = ( ( UINT64 )g_demux_pts * g_demux_header_info.audio_time_scale ) / g_demux_header_info.video_time_scale;

		ret = FJ_Demux_AudioSetup( g_playback_file_id, audio_start_pts );
		UPRINTF_ERR( ret );
	}

	ret = FJ_VideoPlayback_SetStartPTS( g_demux_pts );
	UPRINTF_ERR( ret );

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		ret = FJ_Demux_StreamReadSync( g_playback_file_id, g_demux_header_info.audio_codec, D_USER_UTILITY_PLAYBACK_AUDIO_STREAM_MAX, &demux_stream_param );
		UPRINTF_ERR( ret );

		for( i = 0; i < demux_stream_param.stream_num; i++ ) {
			audio_stream_param[i].stream_end_flag	= demux_stream_param.stream_end_flag[i];
			audio_stream_param[i].stream_size		= demux_stream_param.stream_size[i];
			audio_stream_param[i].stream_address	= demux_stream_param.stream_address[i];
			audio_stream_param[i].pts				= ( ( UINT64 )demux_stream_param.pts[i] * g_demux_header_info.video_time_scale ) / g_demux_header_info.audio_time_scale;
		}

		ret = FJ_VideoPlayback_SetAudioStream( demux_stream_param.stream_num, audio_stream_param );
		UPRINTF_ERR( ret );

		ret = FJ_VideoPlayback_StartAudioDecode();
		UPRINTF_ERR( ret );
	}

	FJ_VideoPlayback_ResumeVideo();

	g_playback_video_status = USER_UTILITY_PLAYBACK_VIDEO_PLAY;

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		g_playback_audio_status = USER_UTILITY_PLAYBACK_AUDIO_ON;
	}

	return OK;
}

static INT32 user_utility_playback_video_trick_play( UINT32 start_pts, E_FJ_MOVIE_PLAY_DIRECTION direction, E_FJ_MOVIE_PLAY_SPEED speed, BOOL pause, BOOL time_search )
{
	INT32											ret;
	E_FJ_DEMUX_DIRECTION							demux_direction;
	E_FJ_DEMUX_SPEED								demux_speed;
	UINT32											read_stream_num;
	UINT32											video_frame_rate_multiply_100;
	T_FJ_DEMUX_STREAM_PARAM							demux_stream_param;
	T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM				video_stream_param[D_USER_UTILITY_PLAYBACK_VIDEO_STREAM_MAX];
	T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM				audio_stream_param[D_USER_UTILITY_PLAYBACK_AUDIO_STREAM_MAX];
	UINT32											audio_start_pts = 0;
	INT32											i;

	if( time_search == TRUE ) {
		if( start_pts % g_demux_header_info.video_time_delta ) {
			return NG;
		}

		if( direction == E_FJ_MOVIE_PLAY_DIRECTION_FWD ) {
			if( ( g_demux_header_info.video_play_last_pts < start_pts ) && ( speed == E_FJ_MOVIE_PLAY_SPEED_1X ) ) {
				return NG;
			}
			else if( ( g_demux_header_info.video_ff_last_pts < start_pts ) && ( ( speed == E_FJ_MOVIE_PLAY_SPEED_2X ) || ( speed == E_FJ_MOVIE_PLAY_SPEED_4X ) || ( speed == E_FJ_MOVIE_PLAY_SPEED_8X ) ) ) {
				return NG;
			}
		}
	}

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_ON ) {
		ret = FJ_VideoPlayback_StopAudioDecode();
		UPRINTF_ERR( ret );

		ret = FJ_VideoPlayback_StopAudioOutput();
		UPRINTF_ERR( ret );

		g_playback_audio_status = USER_UTILITY_PLAYBACK_AUDIO_OFF;
	}

	ret = FJ_VideoPlayback_StopVideoDecode( g_demux_header_info.video_codec, E_FJ_MOVIE_PLAY_LAST_PICTURE_KEEP );
	UPRINTF_ERR( ret );

	switch( direction ) {
		case E_FJ_MOVIE_PLAY_DIRECTION_FWD:
			demux_direction = E_FJ_DEMUX_DIRECTION_FWD;
			break;

		case E_FJ_MOVIE_PLAY_DIRECTION_RWD:
			demux_direction = E_FJ_DEMUX_DIRECTION_RWD;
			break;

		default:
			demux_direction = E_FJ_DEMUX_DIRECTION_FWD;
			break;
	}

	if( time_search == TRUE ) {
		if( direction == E_FJ_MOVIE_PLAY_DIRECTION_FWD ) {
			if( ( g_demux_header_info.video_play_last_pts < start_pts ) && ( speed == E_FJ_MOVIE_PLAY_SPEED_1X ) ) {
				start_pts = g_demux_header_info.video_play_last_pts;
			}
			else if( ( g_demux_header_info.video_ff_last_pts < start_pts ) && ( ( speed == E_FJ_MOVIE_PLAY_SPEED_2X ) || ( speed == E_FJ_MOVIE_PLAY_SPEED_4X ) || ( speed == E_FJ_MOVIE_PLAY_SPEED_8X ) ) ) {
				start_pts = g_demux_header_info.video_ff_last_pts - g_demux_header_info.video_time_delta;
			}
		}
	}

	switch( speed ) {
		case E_FJ_MOVIE_PLAY_SPEED_1X:
			demux_speed = E_FJ_DEMUX_SPEED_1X;
			break;

		case E_FJ_MOVIE_PLAY_SPEED_2X:
			demux_speed = E_FJ_DEMUX_SPEED_2X;
			break;

		case E_FJ_MOVIE_PLAY_SPEED_4X:
			demux_speed = E_FJ_DEMUX_SPEED_4X;
			break;

		case E_FJ_MOVIE_PLAY_SPEED_8X:
			demux_speed = E_FJ_DEMUX_SPEED_8X;
			break;

		case E_FJ_MOVIE_PLAY_SPEED_1_2X:
			demux_speed = E_FJ_DEMUX_SPEED_1_2X;
			break;

		case E_FJ_MOVIE_PLAY_SPEED_1_4X:
			demux_speed = E_FJ_DEMUX_SPEED_1_4X;
			break;

		case E_FJ_MOVIE_PLAY_SPEED_1_8X:
			demux_speed = E_FJ_DEMUX_SPEED_1_8X;
			break;

		default:
			demux_speed = E_FJ_DEMUX_SPEED_1X;
			break;
	}

	ret = FJ_Demux_VideoSetup( g_playback_file_id, start_pts, demux_direction, demux_speed );
	UPRINTF_ERR( ret );

	if( pause != TRUE ) {
		if( ( direction == E_FJ_MOVIE_PLAY_DIRECTION_FWD ) && ( speed == E_FJ_MOVIE_PLAY_SPEED_1X ) ) {
			audio_start_pts = ( ( UINT64 )start_pts * g_demux_header_info.audio_time_scale ) / g_demux_header_info.video_time_scale;
		}
	}

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		if( ( direction == E_FJ_MOVIE_PLAY_DIRECTION_FWD ) && ( speed == E_FJ_MOVIE_PLAY_SPEED_1X ) && ( pause == FALSE ) ) {
			ret = FJ_Demux_AudioSetup( g_playback_file_id, audio_start_pts );
			UPRINTF_ERR( ret );
		}
	}

	if( pause == TRUE ) {
		ret = FJ_VideoPlayback_SetStartPTS( start_pts );
		UPRINTF_ERR( ret );
	}
	else {
		switch( speed ) {
			case E_FJ_MOVIE_PLAY_SPEED_1X:
			case E_FJ_MOVIE_PLAY_SPEED_1_2X:
			case E_FJ_MOVIE_PLAY_SPEED_1_4X:
			case E_FJ_MOVIE_PLAY_SPEED_1_8X:
				ret = FJ_VideoPlayback_SetStartPTS( start_pts );
				UPRINTF_ERR( ret );
				break;

			default :
				break;
		}
	}

	video_frame_rate_multiply_100 = ( g_demux_header_info.video_time_scale * 100 ) / g_demux_header_info.video_time_delta;

	if( video_frame_rate_multiply_100 % 100 ) {
		read_stream_num = ( ( video_frame_rate_multiply_100 / 100 ) + 1 ) / 2;
	}
	else {
		read_stream_num = ( video_frame_rate_multiply_100 / 100 ) / 2;
	}

	if( read_stream_num == 0 ) {
		read_stream_num = 1;
	}

	ret = FJ_Demux_StreamReadSync( g_playback_file_id, g_demux_header_info.video_codec, read_stream_num, &demux_stream_param );
	UPRINTF_ERR( ret );

	for( i = 0; i < demux_stream_param.stream_num; i++ ) {
		video_stream_param[i].stream_end_flag 		= demux_stream_param.stream_end_flag[i];
		video_stream_param[i].stss					= demux_stream_param.stss[i];
		video_stream_param[i].stream_size 			= demux_stream_param.stream_size[i];
		video_stream_param[i].stream_address		= demux_stream_param.stream_address[i];
		video_stream_param[i].pts 					= demux_stream_param.pts[i];
		video_stream_param[i].dts 					= demux_stream_param.dts[i];
	}

	ret = FJ_VideoPlayback_SetVideoStream( g_demux_header_info.video_codec, demux_stream_param.stream_num, video_stream_param );
	UPRINTF_ERR( ret );

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		if( ( direction == E_FJ_MOVIE_PLAY_DIRECTION_FWD ) && ( speed == E_FJ_MOVIE_PLAY_SPEED_1X ) && ( pause == FALSE ) ) {
			ret = FJ_Demux_StreamReadSync( g_playback_file_id, g_demux_header_info.audio_codec, D_USER_UTILITY_PLAYBACK_AUDIO_STREAM_MAX, &demux_stream_param );
			UPRINTF_ERR( ret );

			for( i = 0; i < demux_stream_param.stream_num; i++ ) {
				audio_stream_param[i].stream_end_flag	= demux_stream_param.stream_end_flag[i];
				audio_stream_param[i].stream_size		= demux_stream_param.stream_size[i];
				audio_stream_param[i].stream_address	= demux_stream_param.stream_address[i];
				audio_stream_param[i].pts				= ( ( UINT64 )demux_stream_param.pts[i] * g_demux_header_info.video_time_scale ) / g_demux_header_info.audio_time_scale;
			}

			ret = FJ_VideoPlayback_SetAudioStream( demux_stream_param.stream_num, audio_stream_param );
			UPRINTF_ERR( ret );
		}
	}

	ret = FJ_VideoPlayback_SetSpeedDirection( speed, direction, pause );
	UPRINTF_ERR( ret );

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		if( ( direction == E_FJ_MOVIE_PLAY_DIRECTION_FWD ) && ( speed == E_FJ_MOVIE_PLAY_SPEED_1X ) && ( pause == FALSE ) ) {
			ret = FJ_VideoPlayback_StartAudioDecode();
			UPRINTF_ERR( ret );
		}
	}

	ret = FJ_VideoPlayback_StartVideoDecode(g_demux_header_info.video_codec);
	UPRINTF_ERR( ret );

	if( ( direction == E_FJ_MOVIE_PLAY_DIRECTION_FWD ) && ( speed == E_FJ_MOVIE_PLAY_SPEED_1X ) && ( pause == FALSE ) ) {
		g_playback_video_status = USER_UTILITY_PLAYBACK_VIDEO_PLAY;
	}
	else if( pause == FALSE ) {
		g_playback_video_status = USER_UTILITY_PLAYBACK_VIDEO_TRICK_PLAY;
	}
	else {
		g_playback_video_status = USER_UTILITY_PLAYBACK_VIDEO_FRAME_BY_FRAME;
	}

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_OFF ) {
		if( ( direction == E_FJ_MOVIE_PLAY_DIRECTION_FWD ) && ( speed == E_FJ_MOVIE_PLAY_SPEED_1X ) && ( pause == FALSE ) ) {
			g_playback_audio_status = USER_UTILITY_PLAYBACK_AUDIO_ON;
		}
	}

	return OK;
}

static INT32 user_utility_playback_video_pause( VOID )
{
	INT32 ret;

	ret = FJ_VideoPlayback_PauseVideo();
	UPRINTF_ERR( ret );

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_ON ) {
		ret = FJ_VideoPlayback_StopAudioDecode();
		UPRINTF_ERR( ret );

		if( ret != FJ_ERR_OK ) {
			return NG;
		}

		g_playback_audio_status = USER_UTILITY_PLAYBACK_AUDIO_OFF;
	}

	if( g_playback_video_status == USER_UTILITY_PLAYBACK_VIDEO_PLAY ) {
		g_playback_video_status = USER_UTILITY_PLAYBACK_VIDEO_PAUSE;
	}
	else if( g_playback_video_status == USER_UTILITY_PLAYBACK_VIDEO_TRICK_PLAY ) {
		g_playback_video_status = USER_UTILITY_PLAYBACK_VIDEO_TRICK_PLAY_PAUSE;
	}
	else {
		return NG;
	}

	return OK;
}

static INT32 user_utility_playback_video_stop( VOID )
{
	INT32 ret;

	if( g_playback_audio_status != USER_UTILITY_PLAYBACK_AUDIO_INVALID ) {
		ret = FJ_VideoPlayback_StopAudioDecode();
		UPRINTF_ERR( ret );

		if( ret != FJ_ERR_OK ) {
			return NG;
		}

		g_playback_audio_status = USER_UTILITY_PLAYBACK_AUDIO_OFF;
	}

	ret = FJ_VideoPlayback_StopVideoDecode( g_demux_header_info.video_codec, E_FJ_MOVIE_PLAY_LAST_PICTURE_DONOT_KEEP );
	UPRINTF_ERR( ret );

	if( ret != FJ_ERR_OK ) {
		return NG;
	}

	return OK;
}

static INT32 user_utility_playback_video_end( VOID )
{
	INT32 ret;

	if( g_playback_video_status != USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		if( g_playback_audio_status != USER_UTILITY_PLAYBACK_AUDIO_INVALID ) {
			if( g_audio_end_flag == FALSE ) {
				ret = FJ_VideoPlayback_StopAudioDecode();
				UPRINTF_ERR( ret );

				ret = FJ_VideoPlayback_StopAudioOutput();
				UPRINTF_ERR( ret );

				//wait task stop completely
				OS_User_Dly_Tsk(1);
			}

			ret = FJ_AudioDecoder_Disconnect( g_audio_dec_id, g_audio_out_id );
			UPRINTF_ERR( ret );

			ret = FJ_AudioOut_Close( g_audio_out_id );
			UPRINTF_ERR( ret );

			ret = FJ_AudioDecoder_Close( g_audio_dec_id );
			UPRINTF_ERR( ret );
		}
	}

	ret = FJ_VideoDecoder_Disconnect( g_video_dec_id, g_playback_disp_id, FALSE);
	UPRINTF_ERR( ret );

	ret = FJ_VideoDecoder_Close( g_video_dec_id );
	UPRINTF_ERR( ret );

	ret = FJ_Demux_Finalize( g_playback_file_id );
	UPRINTF_ERR( ret );

	FJ_Mux_Demux_Common_Memory_Finalize();
	FJ_Fs_Close( g_playback_file_id );

	g_playback_video_status = USER_UTILITY_PLAYBACK_VIDEO_OFF;
	g_playback_audio_status = USER_UTILITY_PLAYBACK_AUDIO_OFF;
	g_video_end_flag = FALSE;
	g_audio_end_flag = FALSE;
	g_video_error_flag = FALSE;

	if( g_thumbnail_enter_pressed == TRUE ) {
		ret = User_Utility_Playback_Thumbnail_Key_Enter();
		UPRINTF_ERR( ret );
	}

	return OK;
}

static INT32 user_utility_playback_thumbnail_start( BOOL is_thumbnail_update, UINT32 image_index, UINT16 position_index )
{
	INT32 ret;

	if( is_thumbnail_update == TRUE ) {
		ret = user_utility_playback_thumbnail_display( image_index );
		UPRINTF_ERR( ret );
	}

	ret = user_utility_playback_thumbnail_display_rectangle( position_index, 0xFF00FF00, 2 );
	UPRINTF_ERR( ret );

	return OK;
}

static INT32 user_utility_playback_thumbnail_display( UINT32 image_index )
{
	INT32 ret;

	ret = FJ_SetCurrentImage( image_index );
	UPRINTF_ERR( ret );

	ret = FJ_Playback_ThumbnailIndex();
	UPRINTF_ERR( ret );

	return OK;
}

static INT32 user_utility_playback_thumbnail_display_rectangle( UINT16 position_index, UINT32 color, UINT32 thickness_pix )
{
	INT32 ret;
	UINT32 addr, size = 1;
	UINT32 disp_width;
	UINT32 disp_line;
	T_Rect rect_data;
	T_IconColor color_yuv;

	ret = user_utility_playback_get_thumbnail_info( position_index );
	UPRINTF_ERR( ret );

	if( g_thumbnail_info.thumbnail_num == 0 ) {
		return NG;
	}

	ret = User_Utility_Disp_Get_Resolution_Ratio( g_playback_disp_id, &disp_width, &disp_line );
	UPRINTF_ERR( ret );

	ret = FJ_GetMem( FJ_HOST_HDMI_OSD_FRAME_MEM_LOCATION, &addr, &size );
	UPRINTF_ERR( ret );

	memset( (char *)addr, 0, ( disp_width * disp_line * sizeof( T_IconColor ) ) );

	ret = User_Utility_Disp_OSD_Disable( g_playback_disp_id );
	UPRINTF_ERR( ret );

	ret = User_Utility_Disp_OSD_Set_Window( g_playback_disp_id );
	UPRINTF_ERR( ret );

	rect_data.Postion.StartX = g_thumbnail_info.position[g_thumbnail_info.position_index].x_pos;
	rect_data.Postion.StartY = g_thumbnail_info.position[g_thumbnail_info.position_index].y_pos;
	rect_data.Width = g_thumbnail_info.width;
	rect_data.Height = g_thumbnail_info.lines;
	color_yuv.word = color;
	BF_Osd_Shape_Comm_Draw_Rect_GR(rect_data, disp_width, color_yuv, (ULONG)(addr + rect_data.Postion.StartX * sizeof(T_IconColor) + rect_data.Postion.StartY * disp_width * sizeof(T_IconColor)));

	rect_data.Postion.StartX += thickness_pix;
	rect_data.Postion.StartY += thickness_pix;
	rect_data.Width -= thickness_pix*2;
	rect_data.Height -= thickness_pix*2;
	color_yuv.word = 0;
	BF_Osd_Shape_Comm_Draw_Rect_GR(rect_data, disp_width, color_yuv, (ULONG)(addr + rect_data.Postion.StartX * sizeof(T_IconColor) + rect_data.Postion.StartY * disp_width * sizeof(T_IconColor)));

	ret = User_Utility_Disp_OSD_Render( g_playback_disp_id, addr );
	UPRINTF_ERR( ret );

	ret = User_Utility_Disp_OSD_Enable( g_playback_disp_id );
	UPRINTF_ERR( ret );

	return OK;
}

static INT32 user_utility_playback_thumbnail_key_process( USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_OPERATION key_operation )
{
	INT32 ret;
	USER_UTILITY_PLAYBACK_DIR_OBJ dir_obj = {0};
	UINT16 position_index = 0;
	UINT16 thumbnail_num_max = g_thumbnail_info.line_num * g_thumbnail_info.column_num;

	switch( key_operation ) {
		case USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_UP:
			if( ( INT16 )( g_thumbnail_info.position_index - g_thumbnail_info.column_num ) < 0 ) {
				if( g_thumbnail_info.current_page == 1 ) {
					position_index = g_thumbnail_info.position_index;
					dir_obj.word = g_thumbnail_info.file_list[0].word;
				}
				else {
					position_index = ( g_thumbnail_info.position_index + thumbnail_num_max - g_thumbnail_info.column_num ) % thumbnail_num_max;
					g_thumbnail_info.current_num -= thumbnail_num_max;
					dir_obj.word = g_thumbnail_info.page_list[g_thumbnail_info.current_num / thumbnail_num_max].word;
				}
			}
			else {
				position_index = ( g_thumbnail_info.position_index + thumbnail_num_max - g_thumbnail_info.column_num ) % thumbnail_num_max;
				dir_obj.word = g_thumbnail_info.file_list[0].word;
			}

			break;

		case USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_DOWN:
			if( ( g_thumbnail_info.position_index + g_thumbnail_info.column_num ) >= g_thumbnail_info.thumbnail_num ) {
				if( g_thumbnail_info.current_page == g_thumbnail_info.total_page ) {
					position_index = g_thumbnail_info.position_index;
					dir_obj.word = g_thumbnail_info.file_list[0].word;
				}
				else {
					position_index = ( g_thumbnail_info.position_index + g_thumbnail_info.column_num ) % thumbnail_num_max;
					g_thumbnail_info.current_num += thumbnail_num_max;
					dir_obj.word = g_thumbnail_info.page_list[g_thumbnail_info.current_num / thumbnail_num_max].word;
				}
			}
			else {
				position_index = ( g_thumbnail_info.position_index + g_thumbnail_info.column_num ) % thumbnail_num_max;
				dir_obj.word = g_thumbnail_info.file_list[0].word;
			}

			break;

		case USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_LEFT:
			if( ( INT16 )( g_thumbnail_info.position_index - 1 ) < 0 ) {
				if( g_thumbnail_info.current_page == 1 ) {
					position_index = g_thumbnail_info.position_index;
					dir_obj.word = g_thumbnail_info.file_list[0].word;
				}
				else {
					position_index = ( g_thumbnail_info.position_index + thumbnail_num_max - 1 ) % thumbnail_num_max;
					g_thumbnail_info.current_num -= thumbnail_num_max;
					dir_obj.word = g_thumbnail_info.page_list[g_thumbnail_info.current_num / thumbnail_num_max].word;
				}
			}
			else {
				position_index = ( g_thumbnail_info.position_index + thumbnail_num_max - 1 ) % thumbnail_num_max;
				dir_obj.word = g_thumbnail_info.file_list[0].word;
			}

			break;

		case USER_UTILITY_PLAYBACK_THUMBNAIL_KEY_RIGHT:
			if( ( g_thumbnail_info.position_index + 1 ) >= g_thumbnail_info.thumbnail_num ) {
				if( g_thumbnail_info.current_page == g_thumbnail_info.total_page ) {
					position_index = g_thumbnail_info.position_index;
					dir_obj.word = g_thumbnail_info.file_list[0].word;
				}
				else {
					position_index = ( g_thumbnail_info.position_index + 1 ) % thumbnail_num_max;
					g_thumbnail_info.current_num += thumbnail_num_max;
					dir_obj.word = g_thumbnail_info.page_list[g_thumbnail_info.current_num / thumbnail_num_max].word;
				}
			}
			else {
				position_index = ( g_thumbnail_info.position_index + 1 ) % thumbnail_num_max;
				dir_obj.word = g_thumbnail_info.file_list[0].word;
			}

			break;

		default:
			break;
	}

	if( dir_obj.word == g_thumbnail_info.file_list[0].word ) {
		if( position_index != g_thumbnail_info.position_index ) {
			ret = user_utility_playback_thumbnail_start( FALSE, dir_obj.word, position_index );
			UPRINTF_ERR( ret );
		}
	}
	else {
		if( position_index != g_thumbnail_info.position_index ) {
			ret = user_utility_playback_thumbnail_start( TRUE, dir_obj.word, position_index );
			UPRINTF_ERR( ret );
		}
	}

	return OK;
}

INT32 User_Utility_Playback_AudioOutPlugInCB( UCHAR audio_out_id, T_FJ_AUDIO_OUT_PLUGIN_CB_PARAM * param )
{
	memcpy( ( VOID * )param->out_addr, ( VOID * )param->in_addr, param->in_size );
	param->out_size = param->in_size;
	param->out_ch_type = param->in_ch_type;
	param->out_sampling_rate = param->in_sampling_rate;

	return OK;
}

INT32 User_Utility_Playback_StopWavCB( UCHAR audio_out_id, UCHAR stop_factor )
{
	FJ_AudioOut_ForceClose( audio_out_id );
	UPRINTF("%s(%d) FJ_AudioOut_Close(%d)\n", __func__, __LINE__, audio_out_id);
	return OK;
}

INT32 User_Utility_Playback_GetVideoStreamCB( VOID )
{
	FJ_Demux_StreamRead( g_playback_file_id, g_demux_header_info.video_codec, 2 );

	return OK;
}

INT32 User_Utility_Playback_GetAudioStreamCB( VOID )
{
	if( g_demux_header_info.audio_codec == E_FJ_DEMUX_CODEC_AUDIO_AAC ) {
		FJ_Demux_StreamRead( g_playback_file_id, g_demux_header_info.audio_codec, 2 );
	}
	else {
		FJ_Demux_StreamRead( g_playback_file_id, g_demux_header_info.audio_codec, 1 );
	}

	return OK;
}

INT32 User_Utility_Playback_FirstFrameDisplayCB( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error )
{
	INT32 ret;

	ret = User_Utility_Disp_Enable( g_playback_disp_id );
	UPRINTF_ERR( ret );

	if( decoder_status == E_FJ_MOVIE_PLAY_DECODER_STATUS_NORMAL ) {
		if( g_playback_audio_status != USER_UTILITY_PLAYBACK_AUDIO_INVALID ) {
			ret = FJ_VideoPlayback_StartAudioOutput();
			UPRINTF_ERR( ret );
		}
	}

	return OK;
}

INT32 User_Utility_Playback_VideoDecodeCB( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG display_pts, BOOL error )
{
	if( g_video_error_flag == TRUE ) {
		return NG;
	}

	if( decoder_status == E_FJ_MOVIE_PLAY_DECODER_STATUS_NORMAL ) {
		g_demux_pts = display_pts;
	}
	else if( decoder_status == E_FJ_MOVIE_PLAY_DECODER_STATUS_LAST ) {
		g_video_end_flag = TRUE;
	}

	g_video_error_flag = error;
	Snd_Event_to_User( E_USER_EVENT_PLAY_VIDEO_DECODE, 0 );

	return OK;
}

INT32 User_Utility_Playback_AudioDecodeCB( E_FJ_MOVIE_PLAY_DECODER_STATUS decoder_status, ULLONG decoded_pts, BOOL error )
{
	if( decoder_status == E_FJ_MOVIE_PLAY_DECODER_STATUS_LAST ) {
		g_audio_end_flag = TRUE;
	}

	Snd_Event_to_User( E_USER_EVENT_PLAY_AUDIO_DECODE, 0 );

	return OK;
}

BOOL User_Utility_Playback_StreamReadCompleteCB( UINT32 file_id, E_FJ_DEMUX_CODEC codec, T_FJ_DEMUX_STREAM_PARAM * stream_param )
{
	INT32								ret;
	UINT32								stream_loop;
	T_FJ_MOVIE_PLAY_VIDEO_STREAM_PARAM	video_stream_param[D_USER_UTILITY_PLAYBACK_VIDEO_STREAM_MAX];
	T_FJ_MOVIE_PLAY_AUDIO_STREAM_PARAM	audio_stream_param[D_USER_UTILITY_PLAYBACK_AUDIO_STREAM_MAX];

	switch( codec ) {
		case E_FJ_DEMUX_CODEC_VIDEO_H264:
		case E_FJ_DEMUX_CODEC_VIDEO_H265:
			for( stream_loop = 0; stream_loop < stream_param->stream_num; stream_loop++ ) {
				video_stream_param[stream_loop].stream_end_flag	= stream_param->stream_end_flag[stream_loop];
				video_stream_param[stream_loop].stss			= stream_param->stss[stream_loop];
				video_stream_param[stream_loop].stream_size		= stream_param->stream_size[stream_loop];
				video_stream_param[stream_loop].stream_address	= stream_param->stream_address[stream_loop];
				video_stream_param[stream_loop].pts				= stream_param->pts[stream_loop];
				video_stream_param[stream_loop].dts				= stream_param->dts[stream_loop];

				if( video_stream_param[stream_loop].stream_end_flag == TRUE ) {
					break;
				}
			}

			ret = FJ_VideoPlayback_SetVideoStream( codec, stream_param->stream_num, video_stream_param );

			if( ret != FJ_ERR_OK ) {
				return FALSE;
			}

			break;

		case E_FJ_DEMUX_CODEC_AUDIO_AAC:
		case E_FJ_DEMUX_CODEC_AUDIO_PCM:
			for( stream_loop = 0; stream_loop < stream_param->stream_num; stream_loop++ ) {
				audio_stream_param[stream_loop].stream_end_flag	= stream_param->stream_end_flag[stream_loop];
				audio_stream_param[stream_loop].stream_size		= stream_param->stream_size[stream_loop];
				audio_stream_param[stream_loop].stream_address	= stream_param->stream_address[stream_loop];
				audio_stream_param[stream_loop].pts				= ( stream_param->pts[stream_loop] * g_demux_header_info.video_time_scale ) / g_demux_header_info.audio_time_scale;

				if( audio_stream_param[stream_loop].stream_end_flag == TRUE ) {
					break;
				}
			}

			ret = FJ_VideoPlayback_SetAudioStream( stream_param->stream_num, audio_stream_param );

			if( ret != FJ_ERR_OK ) {
				return FALSE;
			}

			break;

		default:
			return FALSE;
	}

	return TRUE;
}

BOOL User_Utility_Playback_GetStreamAreaCB( UINT32 file_id, E_FJ_DEMUX_CODEC codec, UINT32 * stream_size, UINT32 * stream_address )
{
	INT32 ret = FJ_ERR_NG;

	if( codec == E_FJ_DEMUX_CODEC_VIDEO_H264 ) {
		ret = FJ_VideoPlayback_MemAlloc( E_FJ_MOVIE_PLAY_BUFFER_TYPE_H264_STREAM, stream_size, stream_address );
	}
	else if( codec == E_FJ_DEMUX_CODEC_VIDEO_H265 ) {
		ret = FJ_VideoPlayback_MemAlloc( E_FJ_MOVIE_PLAY_BUFFER_TYPE_H265_STREAM, stream_size, stream_address );
	}
	else if( codec == E_FJ_DEMUX_CODEC_AUDIO_AAC ) {
		ret = FJ_VideoPlayback_MemAlloc( E_FJ_MOVIE_PLAY_BUFFER_TYPE_AAC_STREAM, stream_size, stream_address );
	}
	else {
		ret = FJ_VideoPlayback_MemAlloc( E_FJ_MOVIE_PLAY_BUFFER_TYPE_PCM_STREAM, stream_size, stream_address );
	}

	if( ret != FJ_ERR_OK ) {
		return FALSE;
	}

	return TRUE;
}

INT32 User_Utility_Playback_VideoDecode( VOID )
{
	INT32 ret;

	if( g_playback_video_status == USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	if( ( g_video_error_flag == TRUE )
			|| (g_video_end_flag == TRUE ) ) {
		if( g_thumbnail_enter_pressed == TRUE ) {
			ret = FJ_VideoPlayback_StopVideoDecode( g_demux_header_info.video_codec, E_FJ_MOVIE_PLAY_LAST_PICTURE_DONOT_KEEP );
		}
		else {
			ret = FJ_VideoPlayback_StopVideoDecode( g_demux_header_info.video_codec, E_FJ_MOVIE_PLAY_LAST_PICTURE_KEEP_STOP );
		}

		UPRINTF_ERR( ret );
	}

	if( ( g_video_error_flag == TRUE )
			|| ( ( g_video_end_flag == TRUE ) && ( g_audio_end_flag == TRUE ) )
			|| ( ( g_video_end_flag == TRUE ) && ( g_playback_audio_status != USER_UTILITY_PLAYBACK_AUDIO_ON ) ) ) {
		ret = user_utility_playback_video_end();
		UPRINTF_ERR( ret );
	}

	return OK;
}

INT32 User_Utility_Playback_AudioDecode( VOID )
{
	INT32 ret;

	if( g_playback_video_status == USER_UTILITY_PLAYBACK_VIDEO_OFF ) {
		return NG;
	}

	if( g_audio_end_flag == TRUE ) {
		if( g_playback_audio_status == USER_UTILITY_PLAYBACK_AUDIO_ON ) {
			ret = FJ_VideoPlayback_StopAudioDecode();
			UPRINTF_ERR( ret );

			ret = FJ_VideoPlayback_StopAudioOutput();
			UPRINTF_ERR( ret );

			g_playback_audio_status = USER_UTILITY_PLAYBACK_AUDIO_OFF;
		}
	}

	if( ( g_audio_end_flag == TRUE ) && ( g_video_end_flag == TRUE ) ) {
		ret = user_utility_playback_video_end();
		UPRINTF_ERR( ret );
	}

	return OK;
}

