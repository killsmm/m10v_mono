/**
 * @file		playback.h
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _PLAYBACK_H_
#define _PLAYBACK_H_

#include "ddim_typedef.h"
#include "fj_std.h"
#include "fj_playback.h"
#include "play_macro_wrap.h"


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
/* Function                                                             */
/*----------------------------------------------------------------------*/
//////////////////////////////////
// Playback API (romarea_boot)
//////////////////////////////////
extern FJ_ERR_CODE fj_setcurrentimage( INT32 ImageIndex );
extern INT32       fj_getcurrentimage( VOID );
extern FJ_ERR_CODE fj_setfiletype( USHORT FileType );
extern FJ_ERR_CODE fj_getfileattributes( FILE_ATTRIB* Attribute );
extern FJ_ERR_CODE fj_getimageattributes( IMAGE_ATTRIBUTES* ImageAttr );
extern FJ_ERR_CODE fj_set_pb_autodisplay( E_PB_AUTODISP auto_disp );
extern FJ_ERR_CODE fj_get_pb_autodisplay( E_PB_AUTODISP* auto_disp );
extern FJ_ERR_CODE fj_show_pb_dispimage( E_PB_DISP_DEVICE disp_device );
extern FJ_ERR_CODE fj_get_pb_histogram_size(USHORT* width, USHORT* lines);
extern FJ_ERR_CODE fj_set_pb_display_window( E_PB_DISP_WIN_TYPE window_type, T_PB_DISP_WINDOW* disp_window );
extern FJ_ERR_CODE fj_set_pb_image_window( E_PB_DISP_IMAGE disp_image, T_PB_IMAGE_WINDOW* image_window );
extern FJ_ERR_CODE fj_can_pb_image_window( VOID );

//////////////////////////////////
// Playback API (romarea_last)
//////////////////////////////////
extern FJ_ERR_CODE fj_configthumbindex( THUMBNAIL_CONFIGURATION* Configuration );
//extern FJ_ERR_CODE BF_Playback_SingleDisplay( VOID );
//extern FJ_ERR_CODE BF_Playback_Thumbnailindex( VOID );
extern FJ_ERR_CODE BF_Playback_Set_Main_Image( VOID );
extern FJ_ERR_CODE fj_playback_singledisplay(FJ_MODE screen_mode);
extern FJ_ERR_CODE fj_playback_thumbnailindex(VOID);



#endif // _PLAYBACK_H_
