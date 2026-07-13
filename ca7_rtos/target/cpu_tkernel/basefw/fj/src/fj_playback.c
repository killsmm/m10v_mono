/**
 * @file		fj_playback.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */

/******************************************************************************
 Include
******************************************************************************/
#include "fj_playback.h"
#include "fj_filesystem.h"
#include "mode.h"
#include "play_attr.h"
#include "playback.h"
#include "playback2.h"

/******************************************************************************
 Program
******************************************************************************/

/******************************/
/**  PB Section(FastBoot)    **/
/******************************/

FJ_ERR_CODE FJ_StillPlayback_GetMemInfo( T_FJ_PLAYBACK_MEMINFO* mem_info )
{
	if( BF_Mode_Check(D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}
	
	return fj_stillplayback_getmeminfo( mem_info );
}

FJ_ERR_CODE FJ_StillPlayback_ImageDecode( T_FJ_PLAYBACK_DECODE dec_info, T_FJ_PLAYBACK_YUV_INFO* decode_yuv_info )
{
	if( BF_Mode_Check(D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}

	return fj_stillplayback_imagedecode( &dec_info, decode_yuv_info );
}

FJ_ERR_CODE FJ_StillPlayback_ImageConvert( T_FJ_PLAYBACK_CONVERT conv_info )
{
	if( BF_Mode_Check(D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}

	return fj_stillplayback_imageconvert( &conv_info );
}

FJ_ERR_CODE FJ_SetCurrentImage( INT32 ImageIndex )
{
	// romarea_boot
	if( BF_Mode_Check(D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}

	return fj_setcurrentimage( ImageIndex );
}

INT32 FJ_GetCurrentImage( VOID )
{
	// romarea_boot
	if( BF_Mode_Check(D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK ){
		return 0;	// error return
	}

	return fj_getcurrentimage();
}

FJ_ERR_CODE FJ_SetFileType( USHORT FileType )
{
	// romarea_boot
	if( BF_Mode_Check(D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}

	return fj_setfiletype( FileType );
}

FJ_ERR_CODE FJ_GetFileAttributes( FILE_ATTRIB* Attribute )
{
	// romarea_boot
	if( BF_Mode_Check(D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}

	return fj_getfileattributes( Attribute );
}

FJ_ERR_CODE FJ_GetImageAttributes( IMAGE_ATTRIBUTES* ImageAttr )
{
	// romarea_boot
	if( BF_Mode_Check(D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}

	return fj_getimageattributes( ImageAttr );
}

FJ_ERR_CODE FJ_Show_Pb_DispImage( E_PB_DISP_DEVICE disp_device )
{
	// romarea_boot
	if( BF_Mode_Check(D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}
	
	return fj_show_pb_dispimage( disp_device );
}

FJ_ERR_CODE FJ_Set_Pb_AutoDisplay( E_PB_AUTODISP auto_disp )
{
	// romarea_boot
	if( BF_Mode_Check(D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}
	
	return fj_set_pb_autodisplay( auto_disp );
}

FJ_ERR_CODE FJ_Get_Pb_AutoDisplay( E_PB_AUTODISP* auto_disp )
{
	// romarea_boot
	if( BF_Mode_Check(D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}
	
	return fj_get_pb_autodisplay( auto_disp );
}

FJ_ERR_CODE FJ_Get_Pb_Histogram_Size( USHORT* width, USHORT* lines )
{
	// romarea_last
	if( BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_PB | D_BF_MODE_CATEGORY_RESTORE) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}

	return fj_get_pb_histogram_size( width, lines );
}

FJ_ERR_CODE FJ_SetDisplayWindow( E_PB_DISP_WIN_TYPE window_type, T_PB_DISP_WINDOW* disp_window )
{
	// romarea_boot
	if (BF_Mode_Check( D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL ) != FJ_ERR_OK) {
		return FJ_ERR_NG;	// error return
	}
	
	return fj_set_pb_display_window( window_type, disp_window );
}

FJ_ERR_CODE FJ_SetImageWindow( E_PB_DISP_IMAGE disp_image, T_PB_IMAGE_WINDOW* image_window )
{
	// romarea_boot
	if (BF_Mode_Check( D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL ) != FJ_ERR_OK) {
		return FJ_ERR_NG;	// error return
	}
	
	return fj_set_pb_image_window( disp_image, image_window );
}

FJ_ERR_CODE FJ_CancelImageWindow( VOID )
{
	// romarea_boot
	if (BF_Mode_Check( D_BF_MODE_PB_CODE | D_BF_MODE_CATEGORY_ALL ) != FJ_ERR_OK) {
		return FJ_ERR_NG;	// error return
	}
	
	return fj_can_pb_image_window();
}
/*******************/
/* thumbnail index */
/*******************/
FJ_ERR_CODE FJ_ConfigThumbIndex( THUMBNAIL_CONFIGURATION* Configuration )
{
	// romarea_last
	if( BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL ) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}

	return fj_configthumbindex( Configuration );
}

FJ_ERR_CODE FJ_Playback_ThumbnailIndex( VOID )
{
	// romarea_last
	if( BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL ) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}

	return fj_playback_thumbnailindex();
}

/*******************/
/* Single Display  */
/*******************/
FJ_ERR_CODE FJ_Playback_SingleDisplay( FJ_MODE screen_mode )
{
	// romarea_last
	if( BF_Mode_Check(D_BF_MODE_OTHER_CODE | D_BF_MODE_CATEGORY_ALL ) != FJ_ERR_OK ){
		return FJ_ERR_NG;	// error return
	}

	return fj_playback_singledisplay(screen_mode);
}

