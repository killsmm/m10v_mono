/**
 * @file		play_attr.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "play_attr.h"

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
/* playback attribute data */
T_BF_PLAY_ATTR_MAIN	gPlay_Attr = {
	/* pb_src */
	{
		/* image window main */
		{
			0,					/* pos_x */
			0,					/* pos_y */
			WIDTH_36M,			/* width */
			LINES_36M			/* lines */
		},
		/* image window thumbnail */
		{
			0,					/* pos_x */
			0,					/* pos_y */
			WIDTH_THUMB,		/* width */
			LINES_THUMB			/* lines */
		},
		/* image window cls1 */
		{
			0,					/* pos_x */
			0,					/* pos_y */
			WIDTH_VGA,			/* width */
			LINES_VGA			/* lines */
		},
		/* image window cls2 */
		{
			0,					/* pos_x */
			0,					/* pos_y */
			WIDTH_FHD,			/* width */
			LINES_FHD			/* lines */
		},
	},
	/* pb_disp_win */
	{
		/* disp win play vld */
		{
			0,					/* pos_x */
			0,					/* pos_y */
			WIDTH_VGA,			/* width */
			LINES_VGA,			/* lines */
			FALSE				/* ar_flg */
		},
		/* disp win thumb vld */
		{
			0,					/* pos_x */
			0,					/* pos_y */
			WIDTH_VGA,			/* width */
			LINES_VGA,			/* lines */
			FALSE				/* ar_flg */
		},
		/* disp win play hd */
		{
			0,					/* pos_x */
			0,					/* pos_y */
			WIDTH_FHD,			/* width */
			LINES_FHD,			/* lines */
			FALSE				/* ar_flg */
		},
		/* disp win thumbhd  */
		{
			0,					/* pos_x */
			0,					/* pos_y */
			WIDTH_FHD,			/* width */
			LINES_FHD,			/* lines */
			FALSE				/* ar_flg */
		},
	},
	/* pb */
	{
		/* Set Param */
		0,						/* image_index */
		0,						/* file_type */
		FJ_MODE_DISP_IMAGE_MAIN,/* screen_mode */
		FJ_MODE_DISP_IMAGE_MAIN,/* prev_screen_mode */
		0,						/* thumb_left_lcd */
		0,						/* thumb_top_lcd */
		WIDTH_VGA,				/* thumb_width */
		LINES_VGA,				/* thumb_height */
		0,						/* thumb_left_hdmi */
		0,						/* thumb_top_hdmi */
		WIDTH_FHD,				/* thumb_width_hdmi */
		LINES_FHD,				/* thumb_height_hdmi */
		0x0080,					/* thumb_bgc_yu */
		0x0080,					/* thumb_bgc_yv */
		160,					/* thumbindex draw width for LCD */
		120,					/* thumbindex draw lines for LCD */
		360,					/* thumbindex draw width for HDMI */
		270,					/* thumbindex draw lines for HDMI */
	}
};


/*----------------------------------------------------------------------*/
/* Macro                                                                */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function                                                       */
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Function                                                      */
/*----------------------------------------------------------------------*/
//#pragma arm section code="BASEFW_BOOT_CODE"

/***********************/
/* image win main      */
/***********************/
VOID BF_Play_Attr_Set_Image_Window_Main( T_PB_IMAGE_WINDOW* image_window )
{
	gPlay_Attr.pb_image_win.image_win_main.pos_x = image_window->pos_x;
	gPlay_Attr.pb_image_win.image_win_main.pos_y = image_window->pos_y;
	gPlay_Attr.pb_image_win.image_win_main.width = image_window->width;
	gPlay_Attr.pb_image_win.image_win_main.lines = image_window->lines;
}

VOID BF_Play_Attr_Get_Image_Window_Main( T_PB_IMAGE_WINDOW* image_window )
{
	image_window->pos_x = gPlay_Attr.pb_image_win.image_win_main.pos_x;
	image_window->pos_y = gPlay_Attr.pb_image_win.image_win_main.pos_y;
	image_window->width = gPlay_Attr.pb_image_win.image_win_main.width;
	image_window->lines = gPlay_Attr.pb_image_win.image_win_main.lines;
}

/***********************/
/* image win thumbnail */
/***********************/
VOID BF_Play_Attr_Set_Image_Window_Thumb( T_PB_IMAGE_WINDOW* image_window )
{
	gPlay_Attr.pb_image_win.image_win_thumb.pos_x = image_window->pos_x;
	gPlay_Attr.pb_image_win.image_win_thumb.pos_y = image_window->pos_y;
	gPlay_Attr.pb_image_win.image_win_thumb.width = image_window->width;
	gPlay_Attr.pb_image_win.image_win_thumb.lines = image_window->lines;
}

VOID BF_Play_Attr_Get_Image_Window_Thumb( T_PB_IMAGE_WINDOW* image_window )
{
	image_window->pos_x = gPlay_Attr.pb_image_win.image_win_thumb.pos_x;
	image_window->pos_y = gPlay_Attr.pb_image_win.image_win_thumb.pos_y;
	image_window->width = gPlay_Attr.pb_image_win.image_win_thumb.width;
	image_window->lines = gPlay_Attr.pb_image_win.image_win_thumb.lines;
}

/***********************/
/* image win cls1      */
/***********************/
VOID BF_Play_Attr_Set_Image_Window_CLS1( T_PB_IMAGE_WINDOW* image_window )
{
	gPlay_Attr.pb_image_win.image_win_cls1.pos_x = image_window->pos_x;
	gPlay_Attr.pb_image_win.image_win_cls1.pos_y = image_window->pos_y;
	gPlay_Attr.pb_image_win.image_win_cls1.width = image_window->width;
	gPlay_Attr.pb_image_win.image_win_cls1.lines = image_window->lines;
}

VOID BF_Play_Attr_Get_Image_Window_CLS1( T_PB_IMAGE_WINDOW* image_window )
{
	image_window->pos_x = gPlay_Attr.pb_image_win.image_win_cls1.pos_x;
	image_window->pos_y = gPlay_Attr.pb_image_win.image_win_cls1.pos_y;
	image_window->width = gPlay_Attr.pb_image_win.image_win_cls1.width;
	image_window->lines = gPlay_Attr.pb_image_win.image_win_cls1.lines;
}

/**************************/
/* disp_win_cls2_src */
/**************************/
VOID BF_Play_Attr_Set_Image_Window_CLS2( T_PB_IMAGE_WINDOW* image_window )
{
	gPlay_Attr.pb_image_win.image_win_cls2.pos_x = image_window->pos_x;
	gPlay_Attr.pb_image_win.image_win_cls2.pos_y = image_window->pos_y;
	gPlay_Attr.pb_image_win.image_win_cls2.width = image_window->width;
	gPlay_Attr.pb_image_win.image_win_cls2.lines = image_window->lines;
}

VOID BF_Play_Attr_Get_Image_Window_CLS2( T_PB_IMAGE_WINDOW* image_window )
{
	image_window->pos_x = gPlay_Attr.pb_image_win.image_win_cls2.pos_x;
	image_window->pos_y = gPlay_Attr.pb_image_win.image_win_cls2.pos_y;
	image_window->width = gPlay_Attr.pb_image_win.image_win_cls2.width;
	image_window->lines = gPlay_Attr.pb_image_win.image_win_cls2.lines;
}

/*********************/
/* disp_win_play_lcd */
/*********************/

//#pragma arm section code="BASEFW_COMM_CODE"

VOID BF_Play_Attr_Set_Disp_Win_Play_Lcd_Pos_X( USHORT pos_x )
{
	gPlay_Attr.pb_disp_win.disp_win_play_lcd.pos_x = pos_x;
}

//#pragma arm section code="BASEFW_BOOT_CODE"

USHORT BF_Play_Attr_Get_Disp_Win_Play_Lcd_Pos_X( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_play_lcd.pos_x;
}

//#pragma arm section code="BASEFW_COMM_CODE"

VOID BF_Play_Attr_Set_Disp_Win_Play_Lcd_Pos_Y( USHORT pos_y )
{
	gPlay_Attr.pb_disp_win.disp_win_play_lcd.pos_y = pos_y;
}

//#pragma arm section code="BASEFW_BOOT_CODE"

USHORT BF_Play_Attr_Get_Disp_Win_Play_Lcd_Pos_Y( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_play_lcd.pos_y;
}

//#pragma arm section code="BASEFW_COMM_CODE"

VOID BF_Play_Attr_Set_Disp_Win_Play_Lcd_Width( USHORT width )
{
	gPlay_Attr.pb_disp_win.disp_win_play_lcd.width = width;
}

//#pragma arm section code="BASEFW_BOOT_CODE"

USHORT BF_Play_Attr_Get_Disp_Win_Play_Lcd_Width( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_play_lcd.width;
}

//#pragma arm section code="BASEFW_COMM_CODE"

VOID BF_Play_Attr_Set_Disp_Win_Play_Lcd_Lines( USHORT lines )
{
	gPlay_Attr.pb_disp_win.disp_win_play_lcd.lines = lines;
}

//#pragma arm section code="BASEFW_BOOT_CODE"

USHORT BF_Play_Attr_Get_Disp_Win_Play_Lcd_Lines( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_play_lcd.lines;
}

//#pragma arm section code="BASEFW_COMM_CODE"

VOID BF_Play_Attr_Set_Disp_Win_Play_Lcd_Arflg( BOOL ar_flg )
{
	gPlay_Attr.pb_disp_win.disp_win_play_lcd.ar_flg = ar_flg;
}

//#pragma arm section code="BASEFW_BOOT_CODE"

BOOL BF_Play_Attr_Get_Disp_Win_Play_Lcd_Arflg( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_play_lcd.ar_flg;
}

/**********************/
/* disp_win_thumb_lcd */
/**********************/

//#pragma arm section code="BASEFW_COMM_CODE"

VOID BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Pos_X( USHORT pos_x )
{
	gPlay_Attr.pb_disp_win.disp_win_thumb_lcd.pos_x = pos_x;
}

//#pragma arm section code="BASEFW_BOOT_CODE"

USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Pos_X( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_thumb_lcd.pos_x;
}

//#pragma arm section code="BASEFW_COMM_CODE"

VOID BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Pos_Y( USHORT pos_y )
{
	gPlay_Attr.pb_disp_win.disp_win_thumb_lcd.pos_y = pos_y;
}

//#pragma arm section code="BASEFW_BOOT_CODE"

USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Pos_Y( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_thumb_lcd.pos_y;
}

//#pragma arm section code="BASEFW_COMM_CODE"

VOID BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Width( USHORT width )
{
	gPlay_Attr.pb_disp_win.disp_win_thumb_lcd.width = width;
}

//#pragma arm section code="BASEFW_BOOT_CODE"

USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Width( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_thumb_lcd.width;
}

//#pragma arm section code="BASEFW_COMM_CODE"

VOID BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Lines( USHORT lines )
{
	gPlay_Attr.pb_disp_win.disp_win_thumb_lcd.lines = lines;
}

//#pragma arm section code="BASEFW_BOOT_CODE"

USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Lines( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_thumb_lcd.lines;
}

//#pragma arm section code="BASEFW_COMM_CODE"

VOID BF_Play_Attr_Set_Disp_Win_Thumb_Lcd_Arflg( BOOL ar_flg )
{
	gPlay_Attr.pb_disp_win.disp_win_thumb_lcd.ar_flg = ar_flg;
}

//#pragma arm section code="BASEFW_BOOT_CODE"

BOOL BF_Play_Attr_Get_Disp_Win_Thumb_Lcd_Arflg( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_thumb_lcd.ar_flg;
}

/*********************/
/* disp_win_play_hdmi  */
/*********************/
VOID BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Pos_X( USHORT pos_x )
{
	gPlay_Attr.pb_disp_win.disp_win_play_hdmi.pos_x = pos_x;
}

USHORT BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Pos_X( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_play_hdmi.pos_x;
}

VOID BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Pos_Y( USHORT pos_y )
{
	gPlay_Attr.pb_disp_win.disp_win_play_hdmi.pos_y = pos_y;
}

USHORT BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Pos_Y( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_play_hdmi.pos_y;
}

VOID BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Width( USHORT width )
{
	gPlay_Attr.pb_disp_win.disp_win_play_hdmi.width = width;
}

USHORT BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Width( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_play_hdmi.width;
}

VOID BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Lines( USHORT lines )
{
	gPlay_Attr.pb_disp_win.disp_win_play_hdmi.lines = lines;
}

USHORT BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Lines( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_play_hdmi.lines;
}

VOID BF_Play_Attr_Set_Disp_Win_Play_Hdmi_Arflg( BOOL ar_flg )
{
	gPlay_Attr.pb_disp_win.disp_win_play_hdmi.ar_flg = ar_flg;
}

BOOL BF_Play_Attr_Get_Disp_Win_Play_Hdmi_Arflg( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_play_hdmi.ar_flg;
}

/**********************/
/* disp_win_thumb_hd  */
/**********************/
VOID BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Pos_X( USHORT pos_x )
{
	gPlay_Attr.pb_disp_win.disp_win_thumb_hdmi.pos_x = pos_x;
}

USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Pos_X( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_thumb_hdmi.pos_x;
}

VOID BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Pos_Y( USHORT pos_y )
{
	gPlay_Attr.pb_disp_win.disp_win_thumb_hdmi.pos_y = pos_y;
}

USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Pos_Y( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_thumb_hdmi.pos_y;
}

VOID BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Width( USHORT width )
{
	gPlay_Attr.pb_disp_win.disp_win_thumb_hdmi.width = width;
}

USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Width( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_thumb_hdmi.width;
}

VOID BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Lines( USHORT lines )
{
	gPlay_Attr.pb_disp_win.disp_win_thumb_hdmi.lines = lines;
}

USHORT BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Lines( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_thumb_hdmi.lines;
}

VOID BF_Play_Attr_Set_Disp_Win_Thumb_Hdmi_Arflg( BOOL ar_flg )
{
	gPlay_Attr.pb_disp_win.disp_win_thumb_hdmi.ar_flg = ar_flg;
}

BOOL BF_Play_Attr_Get_Disp_Win_Thumb_Hdmi_Arflg( VOID )
{
	return gPlay_Attr.pb_disp_win.disp_win_thumb_hdmi.ar_flg;
}

/*******************/
/* pb index / type */
/*******************/

//#pragma arm section code="BASEFW_COMM_CODE"

VOID BF_Play_Attr_Set_Image_Index( ULONG image_index )
{
	gPlay_Attr.pb.image_index = image_index;
}

//#pragma arm section code="BASEFW_COMM_CODE"

ULONG BF_Play_Attr_Get_Image_Index( VOID )
{
	return gPlay_Attr.pb.image_index;
}

//#pragma arm section code="BASEFW_COMM_CODE"

VOID BF_Play_Attr_Set_File_Type( USHORT file_type )
{
	gPlay_Attr.pb.file_type = file_type;
}

//#pragma arm section code="BASEFW_COMM_CODE"

USHORT BF_Play_Attr_Get_File_Type( VOID )
{
	return gPlay_Attr.pb.file_type;
}

//#pragma arm section code="BASEFW_BOOT_CODE"

/******************/
/* pb screen mode */
/******************/
VOID BF_Play_Attr_Set_Screen_Mode( FJ_MODE screen_mode )
{
	gPlay_Attr.pb.screen_mode = screen_mode;
}

FJ_MODE BF_Play_Attr_Get_Screen_Mode( VOID )
{
	return gPlay_Attr.pb.screen_mode;
}

VOID BF_Play_Attr_Set_Prev_Screen_Mode( FJ_MODE screen_mode )
{
	gPlay_Attr.pb.prev_screen_mode = screen_mode;
}

FJ_MODE BF_Play_Attr_Get_Prev_Screen_Mode( VOID )
{
	return gPlay_Attr.pb.prev_screen_mode;
}

/***********************/
/* SetParam parameters */
/***********************/
VOID BF_Play_Attr_Set_Thumb_Left_Lcd( USHORT thumb_left_lcd )
{
	gPlay_Attr.pb.thumb_left_lcd = thumb_left_lcd;
}

USHORT BF_Play_Attr_Get_Thumb_Left_Lcd( VOID )
{
	return gPlay_Attr.pb.thumb_left_lcd;
}

VOID BF_Play_Attr_Set_Thumb_Top_Lcd( USHORT thumb_top_lcd )
{
	gPlay_Attr.pb.thumb_top_lcd = thumb_top_lcd;
}

USHORT BF_Play_Attr_Get_Thumb_Top_Lcd( VOID )
{
	return gPlay_Attr.pb.thumb_top_lcd;
}

VOID BF_Play_Attr_Set_Thumb_Width_Lcd( USHORT thumb_width_lcd )
{
	gPlay_Attr.pb.thumb_width_lcd = thumb_width_lcd;
}

USHORT BF_Play_Attr_Get_Thumb_Width_Lcd( VOID )
{
	return gPlay_Attr.pb.thumb_width_lcd;
}

VOID BF_Play_Attr_Set_Thumb_Height_Lcd( USHORT thumb_height_lcd )
{
	gPlay_Attr.pb.thumb_height_lcd = thumb_height_lcd;
}

USHORT BF_Play_Attr_Get_Thumb_Height_Lcd( VOID )
{
	return gPlay_Attr.pb.thumb_height_lcd;
}

VOID BF_Play_Attr_Set_Thumb_Left_Hdmi( USHORT thumb_left_hdmi )
{
	gPlay_Attr.pb.thumb_left_hdmi = thumb_left_hdmi;
}

USHORT BF_Play_Attr_Get_Thumb_Left_Hdmi( VOID )
{
	return gPlay_Attr.pb.thumb_left_hdmi;
}

VOID BF_Play_Attr_Set_Thumb_Top_Hdmi( USHORT thumb_top_hdmi )
{
	gPlay_Attr.pb.thumb_top_hdmi = thumb_top_hdmi;
}

USHORT BF_Play_Attr_Get_Thumb_Top_Hdmi( VOID )
{
	return gPlay_Attr.pb.thumb_top_hdmi;
}

VOID BF_Play_Attr_Set_Thumb_Width_Hdmi( USHORT thumb_width_hdmi )
{
	gPlay_Attr.pb.thumb_width_hdmi = thumb_width_hdmi;
}

USHORT BF_Play_Attr_Get_Thumb_Width_Hdmi( VOID )
{
	return gPlay_Attr.pb.thumb_width_hdmi;
}

VOID BF_Play_Attr_Set_Thumb_Height_Hdmi( USHORT thumb_height_hdmi )
{
	gPlay_Attr.pb.thumb_height_hdmi = thumb_height_hdmi;
}

USHORT BF_Play_Attr_Get_Thumb_Height_Hdmi( VOID )
{
	return gPlay_Attr.pb.thumb_height_hdmi;
}

VOID BF_Play_Attr_Set_Thumb_Bgc_Yu( USHORT thumb_bgc_yu )
{
	gPlay_Attr.pb.thumb_bgc_yu = thumb_bgc_yu;
}

USHORT BF_Play_Attr_Get_Thumb_Bgc_Yu( VOID )
{
	return gPlay_Attr.pb.thumb_bgc_yu;
}

VOID BF_Play_Attr_Set_Thumb_Bgc_Yv( USHORT thumb_bgc_yv )
{
	gPlay_Attr.pb.thumb_bgc_yv = thumb_bgc_yv;
}

USHORT BF_Play_Attr_Get_Thumb_Bgc_Yv( VOID )
{
	return gPlay_Attr.pb.thumb_bgc_yv;
}

VOID BF_Play_Attr_Set_Thumb_Draw_Width_Lcd( USHORT thumb_draw_width_lcd )
{
	gPlay_Attr.pb.thumb_draw_width_lcd = thumb_draw_width_lcd;
}

USHORT BF_Play_Attr_Get_Thumb_Draw_Width_Lcd( VOID )
{
	return gPlay_Attr.pb.thumb_draw_width_lcd;
}

VOID BF_Play_Attr_Set_Thumb_Draw_Lines_Lcd( USHORT thumb_draw_lines_lcd )
{
	gPlay_Attr.pb.thumb_draw_lines_lcd = thumb_draw_lines_lcd;
}

USHORT BF_Play_Attr_Get_Thumb_Draw_Lines_Lcd( VOID )
{
	return gPlay_Attr.pb.thumb_draw_lines_lcd;
}

VOID BF_Play_Attr_Set_Thumb_Draw_Width_Hdmi( USHORT thumb_draw_width_hdmi )
{
	gPlay_Attr.pb.thumb_draw_width_hdmi = thumb_draw_width_hdmi;
}

USHORT BF_Play_Attr_Get_Thumb_Draw_Width_Hdmi( VOID )
{
	return gPlay_Attr.pb.thumb_draw_width_hdmi;
}

VOID BF_Play_Attr_Set_Thumb_Draw_Lines_Hdmi( USHORT thumb_draw_lines_hdmi )
{
	gPlay_Attr.pb.thumb_draw_lines_hdmi = thumb_draw_lines_hdmi;
}

USHORT BF_Play_Attr_Get_Thumb_Draw_Lines_Hdmi( VOID )
{
	return gPlay_Attr.pb.thumb_draw_lines_hdmi;
}

THUMBNAIL_CONFIGURATION * BF_Play_Attr_Get_Thumbnail_Config( VOID )
{
	return &gBF_Play_Index_Thumbnail_Config;
}

VOID * BF_Play_Attr_Get_Thumbnail_Pos( UINT8 disp_device )
{
	return BF_Play_Index_Get_Pos( ( E_BF_PLAY_COMMON_DISP_DEVICE )disp_device );
}

