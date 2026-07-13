/**
 * @file		osd_draw_comm.h
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _OSD_DRAW_COMM_H_
#define _OSD_DRAW_COMM_H_

#include "osd_general.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
typedef ULONG (*Fun_Get_SDRAM_HDMI_HD)();
typedef ULONG (*Fun_Change_SDRAM_HDMI_HD)(VOID);

#define D_BF_OSD_DRAW_COMM_HIST_BOX_PEN_COLOR	(0x06018080)	/**< Default color of histogram frame. */
#define D_BF_OSD_DRAW_COMM_HIST_BACK_COLOR		(0x067B8080)	/**< Default color of histogram background. */
#define D_BF_OSD_DRAW_COMM_HIST_GRAPH_COLOR		(0x0eFF8080)	/**< Default color of histogram graph. */

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Replace color */
typedef enum {
	E_BF_OSD_DRAW_COMM_REPLACE_COLOR_FROM = 0,		/**< Overwritten color */
	E_BF_OSD_DRAW_COMM_REPLACE_COLOR_TO,			/**< Overwrite color */
	E_BF_OSD_DRAW_COMM_REPLACE_COLOR_END			/**< Number of enums */
} E_BF_OSD_DRAW_COMM_REPLACE_COLOR;

/** For skip a color as draw a icon of OC */
typedef enum {
	E_BF_OSD_DRAW_COMM_SKIP_COLOR_ENABLE = 0,		/**< Skip color or not */
	E_BF_OSD_DRAW_COMM_SKIP_COLOR_PALETTE_INDEX,	/**< The color index want to skip */
	E_BF_OSD_DRAW_COMM_SKIP_COLOR_END				/**< Number of enums */
} E_BF_OSD_DRAW_COMM_SKIP_COLOR;

/** Mode group for HDMI */
typedef enum {
	E_BF_OSD_DRAW_COMM_MODE_GROUP4HDMI_VIEW = 0,	/**< View mode */
	E_BF_OSD_DRAW_COMM_MODE_GROUP4HDMI_PB_MENU,		/**< PB/MENU */
	E_BF_OSD_DRAW_COMM_MODE_GROUP4HDMI_OTHER		/**< Other */
} E_BF_OSD_DRAW_COMM_MODE_GROUP4HDMI;

/** Print color */
typedef enum {
	E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR_TRANSPARENCY = 0,			/**< Transparency		*/
	E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR8_BLACK = (int)0x000000FF,	/**< RGBA8888 of Black	*/
	E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR8_RED	= (int)0xFF0000FF,	/**< RGBA8888 of Red	*/
	E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR8_YELLOW = (int)0xFFFF00FF,/**< RGBA8888 of Yellow	*/
	E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR4_BLACK = 0x000F,			/**< RGBA4444 of Black	*/
	E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR4_RED = 0xF00F,			/**< RGBA4444 of Red	*/
	E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR4_YELLOW = 0xFF0F,			/**< RGBA4444 of Yellow	*/
} E_BF_OSD_DRAW_COMM_PRINT_COLOR;

/** Display interface selection */
typedef enum {
	E_FJ_DISP_IFS_NONE = 0,		/**< NONE */
	E_FJ_DISP_IFS_LCD,			/**< LCD  */
	E_FJ_DISP_IFS_HDMI,			/**< HDMI */
	E_FJ_DISP_IFS_MIPI,			/**< MIPI */
} E_FJ_DISP_IFS;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** OSD attribute structure for FJ_SetGDA */
typedef struct {
	struct {
		T_POS left_top_pos;		/**< Coordinates in Vch buffer making it to starting point of drawing.<br>
									StartX : 0~640<br>
									StartY : 0~480 */
	} common;
	struct {
		T_IconColor outline_color;	/**< Color of frame in shape.<br>
								        AYCC.Y : 0~255<br>
								        AYCC.Cb: 0~255<br>
								        AYCC.Cr: 0~255 */
		T_IconColor body_color;		/**< Color of body in shape.<br>
								        AYCC.Y : 0~255<br>
								        AYCC.Cb: 0~255<br>
								        AYCC.Cr: 0~255 */
	} shape;
} T_BF_OSD_DRAW_COMM_GR_GDA_ATTRIBUTE;

/** OSD attribute structure for FJ_SetOSDDisplayAttr */
typedef struct {
	/** OSD common attribute */
	struct {
		T_POS left_top_pos;	/**< Center codinator */
	} common;

	/** Display attribute */
	struct {
		E_FJ_DISP_IFS interface;			/**< Display interface selection */
	} disp;

	/** Color attribute */
	struct {
		T_IconColor	replace[E_BF_OSD_DRAW_COMM_REPLACE_COLOR_END];	/**< Color replaced when drawing */
		BYTE		skip[E_BF_OSD_DRAW_COMM_SKIP_COLOR_END];		/**< Define skip color. skip_color[0]=1: skip 0 not skip. skip_color[1]:skip color index */
		/** Background color for drawing image to Vch/HDch. */
		struct {
			BOOL		enable;	/**< Enable drawing background color. */
			T_IconColor	color;	/**< Background color. */
		} img_back;
	} color;

	/** Shape attribute */
	struct {
		T_IconColor	outline_color;			/**< Color of shap' outline */
		T_IconColor	body_color;				/**< Color of shap' body */
		T_POS		outline_size;			/**< outline size */
		USHORT		round_rect_radius;		/**< Round rectangle's corner's radius */
		USHORT      Donut_Rect_frame_size;	/**< Donut rect frame */
	} shape;

	/** Text attribute */
	struct {
		T_IconColor	body_color;		/**< Color of text' body */
		T_POS		space_size;		/**< space text size */
		T_POS		text_size;		/**< text size */
	} text;

	/** Histogram attribute */
	struct {
		/** LCD */
		struct {
			T_IconColor	pen_color;		/**< Hitogram pen color for LCD */
			T_IconColor	graph_color;	/**< Hitogram graphic color for LCD */
			T_IconColor	back_color;		/**< Hitogram graphic color for LCD */
		} lcd;
		/** HDMI */
		struct {
			T_IconColor	pen_color;		/**< Hitogram pen color for HDMI */
			T_IconColor	graph_color;	/**< Hitogram graphic color for HDMI */
			T_IconColor	back_color;		/**< Hitogram graphic color for HDMI */
		} hdmi;
	} histogram;

	/** HDMI attribute */
	struct {
		E_HD_RESOLUTION	format;		/**< HDMI format */
		BYTE	scan;				/**< Scan mode(Interlace or Progressive) */
		/** Main */
		struct {
			USHORT	global_width	/**< global width */;
			USHORT	width;			/**< width */
			USHORT	lines;			/**< lines */
			Fun_Get_SDRAM_HDMI_HD    pGet_SDRAM_HDch;		/**< The pointer of Get SDRAM's HD display buffer */
			Fun_Change_SDRAM_HDMI_HD pChange_SDRAM_HDch;	/**< The pointer of Change SDRAM's HD display buffer index */
		} main;
		/** OSD */
		struct {
			USHORT	width;			/**< width */
			USHORT	lines;			/**< lines */
		} osd[2];
	} hdmi;

	/** Soft layer attribute */
	struct {
		/** GR */
		struct {
			BYTE blend_ratio[3];	/**< VGR 0/1 and HGR's soft layer blending ratio */
		} gr;
	} soft_layer;

} T_BF_OSD_DRAW_COMM_OSD_ATTRIBUTE;

/** Write information */
typedef struct {
	USHORT		x;					/**< X position */
    USHORT		y;					/**< Y position */
	E_OSDLayer	e_osd_type;			/**< layer ID */
	USHORT		global_size;		/**< global size */
	USHORT		line_feed;			/**< line feed size */
	BYTE*		osd_layer_address;	/**< address */
} T_BF_OSD_DRAW_COMM_WRITE_INFO;

/** Draw shape information */
typedef struct {
	T_BF_SDRAM_MAP_COMMON_OSD drwa_layer;		/**< layer and block */
	T_BF_OSD_DRAW_COMM_WRITE_INFO write_data;	/**< Write information */
} T_BF_OSD_DRAW_COMM_DRAW_SHAPE_INFO;

/** Draw area */
typedef struct {
	T_Rect area_size[OSD_DRAW_AREA_NUM];	/**< Showing area */
	USHORT enable_area;						/**< Showing area enable bit map */
	USHORT vaild_area_num;					/**< Showing area number */
}T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA;


/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#if 0	// MILB Not Use
/** GDA attribute */
extern T_BF_OSD_DRAW_COMM_GR_GDA_ATTRIBUTE gBF_Osd_Draw_Comm_GDA_Attr;
#endif	// MILB Not Use

/** OSD attribute */
extern T_BF_OSD_DRAW_COMM_OSD_ATTRIBUTE gBF_Osd_Draw_Comm_Osd_Attr[E_FJ_DISP_COMM_BLOCK_NUM];

/** Area information */
extern T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA gBF_Osd_Draw_Comm_Showing_Area_Inf[E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_END][E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_END]; 

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
Common fj_setosddisplayattr setting processing in "Made" and "Non-Made".
@param[in] bDisplayID don't care.<br>
@param[in] Attr attribute type.<br>
@param[in] wParam1 is parmater 1.<br>
@param[in] wParam2 is parmater 2.<br>
@param[in] wParam3 is parmater 3.<br>
@param[in] wParam4 is parmater 4.<br>
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
@retval FJ_ERR_NG				Abnormal End.
*/
extern FJ_ERR_CODE BF_Osd_Draw_Comm_Set_Comm_Display_Attr(E_OSDLayer bDisplayID, E_FJ_OSD_ATTR_TYPE Attr, INT32 wParam1, INT32 wParam2, INT32 wParam3, INT32 wParam4);

/**
Get GR No. and block No from layer_id.
@param[in]	layer_id	ID of the layer to be set.<br>
			@ref E_OSDLayer_LCD_OSD0  : LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1  : LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0 : HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1 : HDMI OSD-1 layer<br>
@param[out]	disp_no_data GR No. and block No.<br>
@retval FJ_ERR_OK	Normal End.
@retval Other		See @ref FJ_ERR_CODE.
*/
extern FJ_ERR_CODE BF_Osd_Draw_Comm_Get_Block_GR(E_OSDLayer layer_id, T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data);

/**
Convert the position to draw area's position.
@param[in]	disp_no_data GR No. and block No.<br>
@param[in/out]	positon The position that are converted to draw area positon
*/
extern VOID BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data,T_POS* positon);

/**
Check layer_id is LCD OSD1 layer or not.
@param[in]	layer_id	layer ID.<br>
@retval TRUE	LCD OSD1 layer.
@retval FALSE	Not LCD OSD1 layer.
*/
extern BOOL BF_Osd_Draw_Comm_Is_OSD1_Layer(E_OSDLayer layer_id);

/**
Check layer_id is LCD OSD0 layer or not.
@param[in]	layer_id	layer ID.<br>
@retval TRUE	LCD OSD0 layer.
@retval FALSE	Not LCD OSD0 layer.
*/
extern BOOL BF_Osd_Draw_Comm_Is_OSD0_Layer(E_OSDLayer layer_id);

/**
Check layer_id is HDMI OSD1 layer or not.
@param[in]	layer_id	layer ID.<br>
@retval TRUE	HDMI OSD1 layer.
@retval FALSE	Not HDMI OSD1 layer.
*/
extern BOOL BF_Osd_Draw_Comm_Is_HDMI_OSD1_Layer(E_OSDLayer layer_id);

/**
Check layer_id is HDMI OSD0 layer or not.
@param[in]	layer_id	layer ID.<br>
@retval TRUE	HDMI OSD0 layer.
@retval FALSE	Not HDMI OSD0 layer.
*/
extern BOOL BF_Osd_Draw_Comm_Is_HDMI_OSD0_Layer(E_OSDLayer layer_id);

/**
The text for debugging is displayed on VGR.<br>
Because the font data for debugging is used, it is not necessary to load the resource file.<br>
@param[in] color	color[0] is specification of the background color.<br>
					color[1] is specification of the font color.<br>
@param[in] postion	Coordinates at drawing position. <br>
@param[in] string	String where it wants to draw.<br>
					This function corresponds to all ASCII codes.<br>
@param[in] screen_width	draw area's size

@remarks	In the specification of the color in drawing to OC, the following Palette-OS_USER_ID are used.<br>
			Please set the color to these Palette-OS_USER_ID beforehand by the @ref FJ_SetOSDCLUTEx function.<br>
			@ref FJ_OSD_PRINT_COLOR_TRANSPARENCY : Palette No.252<br>
			@ref FJ_OSD_PRINT_COLOR_BLACK        : Palette No.253<br>
			@ref FJ_OSD_PRINT_COLOR_RED          : Palette No.254<br>
			@ref FJ_OSD_PRINT_COLOR_YELLOW       : Palette No.255<br>
*/
extern VOID BF_Osd_Draw_Comm_Print_String_GR(ULONG vgr_address, UINT32 color[FJ_OSD_PRINT_COLOR_INDEX_END], 
											const CHAR* string, UINT32 screen_width, UINT32 screen_height, T_POS postion);

/**
Get OSD layer address
@param[in]	pos  position
@param[in]	e_osd_type OSD layer type
@param[in]	osd_size osd size(O)
@param[in]	osd_layer_address OSD layer address
*/
extern VOID BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(T_POS const* const pos, ULONG* osd_layer_address,const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data);

/**
Judge the if switch display buffer.
@param[in] mode New mode which will be gone ON<br>
@retval E_BF_OSD_DRAW_COMM_MODE_GROUP4HDMI_VIEW		View mode.
@retval E_BF_OSD_DRAW_COMM_MODE_GROUP4HDMI_PB_MENU	PB/MENU.
@retval E_BF_OSD_DRAW_COMM_MODE_GROUP4HDMI_OTHER	Other.
*/
extern E_BF_OSD_DRAW_COMM_MODE_GROUP4HDMI BF_Osd_Draw_Comm_Get_Mode_Group(FJ_MODE mode);

/**
Get the overlap rectangle of 2 rectangles
@param[in]	rect1	Rectangle1
@param[in]	rect2	Rectangle2
@param[out]	overlap_rect	overlap rect
@retval TRUE	overlap.
@retval FALSE	Not any part of overlap.
*/
extern BOOL BF_Osd_Draw_Comm_Get_Overlap_Rect(T_Rect rect1, T_Rect rect2, T_Rect* overlap_rect);

/**
Set the rectangle to the one inside of LCD
@param[in]	window_size display size. [0]:width [1]:height
@param[in]	dispWnd	rectangel
@retval TRUE	The rectangle is in the rectangle.
@retval FALSE	The rectangle is not in the rectangle.
 */
extern BOOL BF_Osd_Draw_Comm_Is_Rect_At_LCD_Inside(const USHORT window_size[2], T_Rect* dispWnd);

/**
It is acquired whether rect is an inner side of rect. 
@param[in]	disp_rect base rect.
@param[in]	draw_rect draw rect.
@retval TRUE	The rectangle is in the rectangle.
@retval FALSE	The rectangle is not in the rectangle.
*/
extern BOOL BF_Osd_Draw_Comm_Is_Rect_Inside(T_Rect disp_rect, T_Rect draw_rect);

/**
It is acquired whether rect is an inner side of rect. 
@param[in]	disp_no_data GR No. and block No.<br>
@param[in]	draw_pos draw position.
@param[in]	draw_width draw width.
@param[in]	draw_height draw height.
@retval TRUE	The rectangle is in the rectangle.
@retval FALSE	The rectangle is not in the rectangle.
*/
extern FJ_ERR_CODE BF_Osd_Draw_Comm_Is_Rect_Draw_Area_Inside(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, 
													 T_POS draw_pos, 	
													 SHORT draw_width,
													 SHORT draw_height);

/**
Specified size and positions check
@param[in]	uStartX start position X
@param[in]	uStartY start position Y
@param[in]	uWidth data width
@param[in]	uHight data height
@retval FJ_ERR_OK		Normal End.
@retval FJ_ERR_POSITION	Abnormal End. Input parameter error.
*/
extern FJ_ERR_CODE BF_Osd_Draw_Comm_Check_Draw_Size_Pos_For_YCC420(USHORT uStartX, USHORT uStartY, USHORT uWidth, USHORT uHight);

/**
Blend destination cuurent pixel with icon pixel to destination cuurent pixel
@param[in] 	iconCurPixel icon pixel data	
@param[in] 	alpha maximum alpha	
@param[in/out] destCurPixel destination cuurent pixel's pointer
@attention	if iconCurrentPixel's alpha=0  or alpha=0, this function would not be called.
*/
extern VOID BF_Osd_Draw_Comm_Blend_Rgba(OSD_USHORT_INT iconCurrentPixel, OSD_USHORT_INT* destCurrrentPixel, BYTE alpha);

/**
Copy a box area the another
@param[in]	src_addr Copy source address
@param[in]	src_gl_width global width of source
@param[in]	dst_gl_width global width of destination
@param[in]	dst_addr copy destination Address
@param[in]	width box width
@param[in]	lines box lines
@param[in]	alpha Blend rate
*/
extern VOID BF_Osd_Draw_Comm_Blend_Box_To_Box(OSD_USHORT_INT* src_addr, USHORT src_gl_width,
									USHORT dst_gl_width, OSD_USHORT_INT* dst_addr,
									USHORT width, USHORT lines
									,BYTE alpha);

/**
Get YCbCr buffer address of HD.
@param[out]	hd_address YCbCr buffer address
@return		HD's display size
@attention	HDMI should be connecting
*/
extern UINT32 BF_Osd_Draw_Comm_Get_HD_Draw_Buffer_Addr(T_IM_DISP_IMAGE_ADDR* hd_address);

/**
Get color value.
@param[in]	oc_color	Palette index.
@return		color value
*/
extern E_BF_OSD_DRAW_COMM_PRINT_COLOR BF_Osd_Draw_Comm_Get_GR_Print_Color(FJ_OSD_PRINT_COLOR oc_color);


#endif // _OSD_DRAW_COMM_H_
