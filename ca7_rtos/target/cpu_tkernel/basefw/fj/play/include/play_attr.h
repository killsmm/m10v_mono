/**
 * @file		play_attr.h
 * @brief		play attribute
 * @note		None
 * @attention	If image_procedure is disabled, direct call some APIs on image_procedure.
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _PLAY_ATTR_H_
#define _PLAY_ATTR_H_

#include "fj_playback.h"
#include "sdram_map_common.h"
#include "play_index.h"

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
// Play Back Attribute Data
typedef struct {
	/* main image pos/size */
	T_PB_IMAGE_WINDOW	image_win_main;
	/* thumbnail image pos/size */
	T_PB_IMAGE_WINDOW	image_win_thumb;
	/* cls1 image pos/size */
	T_PB_IMAGE_WINDOW	image_win_cls1;
	/* cls2 image pos/size */
	T_PB_IMAGE_WINDOW	image_win_cls2;
} T_BF_PLAY_ATTR_IMAGE_WIN;

typedef struct {
	/* play window */
	T_PB_DISP_WINDOW	disp_win_play_lcd;
	/* thumb window */
	T_PB_DISP_WINDOW	disp_win_thumb_lcd;
	/* play window */
	T_PB_DISP_WINDOW	disp_win_play_hdmi;
	/* thumb window */
	T_PB_DISP_WINDOW	disp_win_thumb_hdmi;
} T_BF_PLAY_ATTR_DISP_WIN;

typedef struct{
	// pb index / type
	ULONG			image_index;
	USHORT			file_type;
	// pb screen mode
	FJ_MODE			screen_mode;
	FJ_MODE			prev_screen_mode;
	// SetParam
	USHORT			thumb_left_lcd;
	USHORT			thumb_top_lcd;
	USHORT			thumb_width_lcd;
	USHORT			thumb_height_lcd;
	USHORT			thumb_left_hdmi;
	USHORT			thumb_top_hdmi;
	USHORT			thumb_width_hdmi;
	USHORT			thumb_height_hdmi;
	USHORT			thumb_bgc_yu;
	USHORT			thumb_bgc_yv;
	USHORT			thumb_draw_width_lcd;
	USHORT			thumb_draw_lines_lcd;
	USHORT			thumb_draw_width_hdmi;
	USHORT			thumb_draw_lines_hdmi;
} T_BF_PLAY_ATTR_DATA ;

typedef struct{
	T_BF_PLAY_ATTR_IMAGE_WIN	pb_image_win;
	T_BF_PLAY_ATTR_DISP_WIN		pb_disp_win;
	T_BF_PLAY_ATTR_DATA			pb;
} T_BF_PLAY_ATTR_MAIN ;


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
/*************************/
/* source image pos/size */
/*************************/
// image window main
VOID	BF_Play_Attr_Set_Image_Window_Main( T_PB_IMAGE_WINDOW* image_window );
VOID	BF_Play_Attr_Get_Image_Window_Main( T_PB_IMAGE_WINDOW* image_window );

// image window thumbnail
VOID	BF_Play_Attr_Set_Image_Window_Thumb( T_PB_IMAGE_WINDOW* image_window );
VOID	BF_Play_Attr_Get_Image_Window_Thumb( T_PB_IMAGE_WINDOW* image_window );

// image window cls1
VOID	BF_Play_Attr_Set_Image_Window_CLS1( T_PB_IMAGE_WINDOW* image_window );
VOID	BF_Play_Attr_Get_Image_Window_CLS1( T_PB_IMAGE_WINDOW* image_window );

// image window cls2
VOID	BF_Play_Attr_Set_Image_Window_CLS2( T_PB_IMAGE_WINDOW* image_window );
VOID	BF_Play_Attr_Get_Image_Window_CLS2( T_PB_IMAGE_WINDOW* image_window );

/*******************************/
/* destination window pos/size */
/*******************************/
// disp_win_play_lcd
VOID   BF_Play_Attr_Set_Disp_Win_Play_Lcd_Pos_X( USHORT pos_x );
USHORT BF_Play_Attr_Get_Disp_Win_Play_Lcd_Pos_X( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Play_Lcd_Pos_Y( USHORT pos_y );
USHORT BF_Play_Attr_Get_Disp_Win_Play_Lcd_Pos_Y( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Play_Lcd_Width( USHORT width );
USHORT BF_Play_Attr_Get_Disp_Win_Play_Lcd_Width( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Play_Lcd_Lines( USHORT lines );
USHORT BF_Play_Attr_Get_Disp_Win_Play_Lcd_Lines( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Play_Lcd_Arflg( BOOL ar_flg );
BOOL   BF_Play_Attr_Get_Disp_Win_Play_Lcd_Arflg( VOID );

// disp_win_thumb_lcd
VOID   BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Pos_X( USHORT pos_x );
USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Pos_X( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Pos_Y( USHORT pos_y );
USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Pos_Y( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Width( USHORT width );
USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Width( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Lines( USHORT lines );
USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Lines( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Arflg( BOOL ar_flg );
BOOL   BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Arflg( VOID );

// disp_win_play_hdmi
VOID   BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Pos_X( USHORT pos_x );
USHORT BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Pos_X( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Pos_Y( USHORT pos_y );
USHORT BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Pos_Y( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Width( USHORT width );
USHORT BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Width( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Lines( USHORT lines );
USHORT BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Lines( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Arflg( BOOL ar_flg );
BOOL   BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Arflg( VOID );

// disp_win_thumb_hdmi
VOID   BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Pos_X( USHORT pos_x );
USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Pos_X( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Pos_Y( USHORT pos_y );
USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Pos_Y( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Width( USHORT width );
USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Width( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Lines( USHORT lines );
USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Lines( VOID );
VOID   BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Arflg( BOOL ar_flg );
BOOL   BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Arflg( VOID );

// pb index / type
VOID   BF_Play_Attr_Set_Image_Index( ULONG image_index );
ULONG  BF_Play_Attr_Get_Image_Index( VOID );
VOID   BF_Play_Attr_Set_File_Type( USHORT file_type );
USHORT BF_Play_Attr_Get_File_Type( VOID );

// pb screen mode
VOID    BF_Play_Attr_Set_Screen_Mode( FJ_MODE screen_mode );
FJ_MODE BF_Play_Attr_Get_Screen_Mode( VOID );
VOID    BF_Play_Attr_Set_Prev_Screen_Mode( FJ_MODE screen_mode );
FJ_MODE BF_Play_Attr_Get_Prev_Screen_Mode( VOID );

// SetParam parameters
VOID   BF_Play_Attr_Set_Thumb_Left_Lcd( USHORT thumb_left_lcd );
USHORT BF_Play_Attr_Get_Thumb_Left_Lcd( VOID );
VOID   BF_Play_Attr_Set_Thumb_Top_Lcd( USHORT thumb_top_lcd );
USHORT BF_Play_Attr_Get_Thumb_Top_Lcd( VOID );
VOID   BF_Play_Attr_Set_Thumb_Width_Lcd( USHORT thumb_width_lcd );
USHORT BF_Play_Attr_Get_Thumb_Width_Lcd( VOID );
VOID   BF_Play_Attr_Set_Thumb_Height_Lcd( USHORT thumb_height_lcd );
USHORT BF_Play_Attr_Get_Thumb_Height_Lcd( VOID );

VOID   BF_Play_Attr_Set_Thumb_Left_Hdmi( USHORT thumb_left_hdmi );
USHORT BF_Play_Attr_Get_Thumb_Left_Hdmi( VOID );
VOID   BF_Play_Attr_Set_Thumb_Top_Hdmi( USHORT thumb_top_hdmi );
USHORT BF_Play_Attr_Get_Thumb_Top_Hdmi( VOID );
VOID   BF_Play_Attr_Set_Thumb_Width_Hdmi( USHORT thumb_width_hdmi );
USHORT BF_Play_Attr_Get_Thumb_Width_Hdmi( VOID );
VOID   BF_Play_Attr_Set_Thumb_Height_Hdmi( USHORT thumb_height_hdmi );
USHORT BF_Play_Attr_Get_Thumb_Height_Hdmi( VOID );

VOID   BF_Play_Attr_Set_Thumb_Bgc_Yu( USHORT thumb_bgc_yu );
USHORT BF_Play_Attr_Get_Thumb_Bgc_Yu( VOID );
VOID   BF_Play_Attr_Set_Thumb_Bgc_Yv( USHORT thumb_bgc_yv );
USHORT BF_Play_Attr_Get_Thumb_Bgc_Yv( VOID );
VOID   BF_Play_Attr_Set_Thumb_Draw_Width_Lcd( USHORT thumb_draw_width_lcd );
USHORT BF_Play_Attr_Get_Thumb_Draw_Width_Lcd( VOID );
VOID   BF_Play_Attr_Set_Thumb_Draw_Lines_Lcd( USHORT thumb_draw_lines );
USHORT BF_Play_Attr_Get_Thumb_Draw_Lines_Lcd( VOID );
VOID   BF_Play_Attr_Set_Thumb_Draw_Width_Hdmi( USHORT thumb_draw_width_hdmi );
USHORT BF_Play_Attr_Get_Thumb_Draw_Width_Hdmi( VOID );
VOID   BF_Play_Attr_Set_Thumb_Draw_Lines_Hdmi( USHORT thumb_draw_lines_hdmi );
USHORT BF_Play_Attr_Get_Thumb_Draw_Lines_Hdmi( VOID );

THUMBNAIL_CONFIGURATION * BF_Play_Attr_Get_Thumbnail_Config( VOID );
VOID * BF_Play_Attr_Get_Thumbnail_Pos( UINT8 disp_device );
#endif

