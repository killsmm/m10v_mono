/**
 * @file		osd_shape_comm.h
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _OSD_SHAPE_COMM_H_
#define _OSD_SHAPE_COMM_H_

#include "osd_draw_comm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
/** Shape parts */
typedef enum {
	E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE = 0,		/**< Outline of shape */
	E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY,				/**< Body of shape */
	E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_END					/**< End flag */
} E_BF_OSD_SHAPE_COMM_SHAPE_PARTS;

/** Quadrant sequence */
typedef enum {
	// Don't change the order
	E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_TOP = 0,	/**< Circle right top */
	E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_TOP,		/**< Circle left top */
	E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_BOTTOM,	/**< Circle left bottom */
	E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_BOTTOM,	/**< Circle right bottom */
	E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_TOP_FRAME,		/**< Rectangle top frame */
	E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_BOTTOM_FRAME,	/**< Rectangle bottom frame */
	E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_LEFT_FRAME,		/**< Rectangle left frame */
	E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_RIGHT_FRAME,		/**< Rectangle right frame */
	E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_TOP_BODY,		/**< Rectangle top body */
	E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_CENCER,			/**< Rectangle center */
	E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_BOTTOM_BODY,		/**< Rectangle bottom body */
	E_BF_OSD_SHAPE_COMM_ROUND_POS_END					/**< End flag */
} E_BF_OSD_SHAPE_COMM_ROUND_POS;

/** Quadrant */
typedef enum {
	E_BF_OSD_SHAPE_COMM_QUADRANT_ONE = (BYTE)0,			/**< an arc in No 1 quadrant */
	E_BF_OSD_SHAPE_COMM_QUADRANT_TWO,					/**< an arc in No 2 quadrant */
	E_BF_OSD_SHAPE_COMM_QUADRANT_THREE,					/**< an arc in No 3 quadrant */
	E_BF_OSD_SHAPE_COMM_QUADRANT_FOUR,					/**< an arc in No 4 quadrant */
	E_BF_OSD_SHAPE_COMM_QUADRANT_ALL					/**< circle */
}E_BF_OSD_SHAPE_COMM_QUADRANT;

/** Rectangle */
typedef enum {
	// Don't change the order
	E_BF_OSD_SHAPE_COMM_DIV_RECT_CENTER = 0,			/**< center */
	E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT,					/**< left */
	E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT,					/**< right */
	E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP,					/**< top */
	E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM,				/**< bottom */
	E_BF_OSD_SHAPE_COMM_DIV_RECT_END					/**< end */
}E_BF_OSD_SHAPE_COMM_DIV_RECT;

/** Quadrant sequence */
typedef enum {
	// Don't change the order
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_TOP = 0,/**< exterior top */
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_TOP,	/**< interior top */
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_BOTTOM,	/**< interior bottom */
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_BOTTOM,	/**< exterior bottom */
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_LEFT,	/**< exterior left */
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_LEFT,	/**< interior left */
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_RIGHT,	/**< interior right */
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_RIGHT,	/**< exterior right */
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_TOP,		/**< center top */
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_BOTTOM,	/**< center bottom */
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_LEFT,		/**< center left */
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_RIGHT,	/**< center right */
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER,			/**< center */
	E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_THE_END			/**< end */
}E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT;

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
/** Shape information */
typedef struct {
	E_FJ_OSD_SHAPE_TYPE	wType;	/**< Shape type */
	T_Rect				Rect;	/**< Shape data */
} T_BF_OSD_SHAPE_COMM_SHAPE_INFO;

/** Color and shape information */
typedef struct {
	T_BF_OSD_SHAPE_COMM_SHAPE_INFO	rect_data;		/**< Shape information */
	T_IconColor						Color_Blending;	/**< color */
}T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO;

/** Ring */
typedef struct {
	USHORT	Circle_Start1;	/**< start 1 */
	USHORT	Circle_End1;	/**< end 1 */
	USHORT	Circle_Start2;	/**< start 2 */
	USHORT	Circle_End2;	/**< end 2 */
}T_BF_OSD_SHAPE_COMM_RING;

/** Round corner */
typedef struct {
	T_BF_OSD_SHAPE_COMM_SHAPE_INFO	round_corner_data[E_BF_OSD_SHAPE_COMM_QUADRANT_ALL];	/**< left/right corner data */
	T_BF_OSD_SHAPE_COMM_RING*		circle_frame_buffer_left;	/**< left ring */
	T_BF_OSD_SHAPE_COMM_RING*		circle_frame_buffer_right;	/**< right ring */
} T_BF_OSD_SHAPE_COMM_ROUND_CORNER;

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
Check the parameter. The position must be even if at VGR
@param[in]	width	width
@param[in]	heigh	heigh
@return 0	OK
*/
extern FJ_ERR_CODE BF_Osd_Shape_Comm_Check_Coordinates(USHORT width, USHORT heigh);

/**
@brief Draw shap on osd by display id and type
@param[in]	disp_no_data : Draw area information
@param[in]	wType	shape type<br>
			FUJI_OSD_SHAPE_RECT<br>
			FUJI_OSD_SHAPE_RECT_FRAME<br>
			FUJI_OSD_SHAPE_FILLED_RECT_FRAME<br>
			FUJI_OSD_SHAPE_CIRCLE<br>
			FUJI_OSD_SHAPE_FILLED_CIRCLE<br>
			FUJI_OSD_SHAPE_ROUND_FRAME<br>
			FUJI_OSD_SHAPE_FILLED_ROUND_FRAME<br>
@param[in]	obj_wnd		display object 's position and width/height
@param[in]	dispWnd		Structure that define the partial display window (x, y, width, height)
@param[in]	Color_Blending	Pen and brush color
@param[in]	frame_theckness	frame size
@param[in]	DrawMode	Specified to whether blend with current drawing when draw a shape
@param[in]	alpha	alpha blend
@retval		0	OK
@retval		-1	wStartX is not even error
@retval		2	E_ERROR_CODE_UWWIDTH_NO_EVEN
@note		wStartX and uwWidth must be even
@attention	When E_FJ_OSD_SHAPE_TYPE_RECT is set and body color is transparement and uwWidth*uwHeight=0
			the range of rectangle will be cleared
*/
extern FJ_ERR_CODE BF_Osd_Shape_Comm_Draw_Shape_GR(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, 
						E_FJ_OSD_SHAPE_TYPE wType, T_Rect obj_wnd, T_Rect dispWnd,
						const T_IconColor Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_END],
						T_POS const* const frame_theckness, E_DrawMode DrawMode, BYTE alpha );

/**
draw rectangle
@param[in]	rect_data	shape's data
@param[in]	grihsize	GR ch' horizontal size
@param[in]	color_yuv	color data
@param[in]	osd_layer_address	draw OSD layer
*/
extern VOID BF_Osd_Shape_Comm_Draw_Rect_GR(T_Rect rect_data, USHORT grihsize, T_IconColor color_yuv, ULONG osd_layer_address);

/**
Draw rectangular triangle
@param[in] disp_no_data is layer information.<br>
@param[in] triangle_x is X coordinate.<br>
@param[in] triangle_y is Y coordinate.<br>
@param[in] width is width.<br>
@param[in] heigh is height.<br>
@param[in] type is type of the rectangular triangle as following.<br>
		@ref E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_LT
		@ref E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_RT
		@ref E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_LB
		@ref E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_RB
@retval FJ_ERR_OK Normal End.
@retval FJ_ERR_NG_INPUT_PARAM Parameter error.
@retval FJ_ERR_POSITION In drawing to VGR, coordinates or size is not an even number.
@retval FJ_ERR_OSD_SEQUENCE In drawing to OC, CLUT is not loaded.
@retval FJ_ERR_OSD_LAYER The specification of the layer is illegal.
@retval FJ_ERR_NG	Abnormal End.
@remarks When drawing to VGR, triangle_x, triangle_y, width, heigh must be even number.<br>
*/
extern FJ_ERR_CODE BF_Osd_Shape_Comm_Draw_Triangle(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, USHORT triangle_x, USHORT triangle_y, USHORT width, USHORT heigh, E_FJ_OSD_SHAPE_TYPE type);

/**
Divide a rectangle to 5 rectangle
@param[in]	rect_data	shape's data
@param[in]	Outline_Size	Ouline size
@param[in]	display_data	divide result
*/
extern VOID BF_Osd_Shape_Comm_Divide_Rect_5_Parts(T_BF_OSD_SHAPE_COMM_SHAPE_INFO const* const rect_data, T_POS const* const Outline_Size, T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO display_data[5]);

/**
Divide the round rectangle to 7 parts(4 circles and 3 rectangle)
@param[in]	disp_no_data	block information
@param[in]	round_rect	round data
@param[out]	round_data_7_parts
@param[in]	frame_size	frame size
@return 0	OK
@note		round rectangle's size must be divisible by 8
*/
extern FJ_ERR_CODE BF_Osd_Shape_Comm_Divide_Round_11_Parts(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no, T_BF_OSD_SHAPE_COMM_SHAPE_INFO const* const round_rect,
														   T_BF_OSD_SHAPE_COMM_SHAPE_INFO round_data_7_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_END], T_POS const* const frame_size);

/**
Divide a rectangle to 13 rectangle
@param[in]	disp_no_data	block information
@param[in]	rect_data	shape's data
@param[in]	Outline_Size	Ouline size
@param[in]	display_data	divide result
*/
extern VOID BF_Osd_Shape_Comm_Divide_Donut_13_Parts(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no,
													T_BF_OSD_SHAPE_COMM_SHAPE_INFO const* const rect_data, T_POS const* const Outline_Size,
													T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_THE_END]);

/**
Draw a donut shape that boder size is 4 on VGA size
@param[in]	in_addr	draw address
@param[in]	in_x	position of X
@param[in]	in_y	position of Y
@param[in]	in_w	Width
@param[in]	in_h	Height
@param[in]	in_edge	outline color of Y
@param[in]	in_main	body's color of Y
@param[in]	in_cc1	body's color of U
@param[in]	in_cc2	body's color of V
@note	the U/V of outline must be as same as body
*/
extern VOID BF_Osd_Shape_Comm_Draw_2color_Frame(const ULONG in_addr[2],
					 ULONG in_x, ULONG in_y, ULONG in_w, ULONG in_h, ULONG in_gw,
					 BYTE in_edge, BYTE in_main, BYTE in_cc1, BYTE in_cc2);

/**
Get line corner position from centern
@param[in]	disp_no_data	layer and block information
@param[in]	obj_wnd		window
@retval		FJ_ERR_OK				Normal End
*/
extern FJ_ERR_CODE BF_Osd_Shape_Comm_Draw_Circle(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, T_Rect obj_wnd);

/**
Draw a straight line
@param[in]	disp_no_data	layer and block information
@param[in]	line	the line terminal point(start_pos.StartX, start_pos.StartY, end_pos.StartX, end_pos.StartY)
@param[in]	Thickness	the line width
@retval		FJ_ERR_OK				Normal End
@retval		FJ_ERR_OSD_LAYER		Abnormal End. bDisplayID is illegal.
@retval		FJ_ERR_OSD_SEQUENCE		Abnormal End. Icon, font, string or CLUT data has not been load.
*/
extern FJ_ERR_CODE BF_Osd_Shape_Comm_drawstraight(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, const FJ_LINE_COORDINATES* line, USHORT Thickness);

/**
Draw a slant line
@param[in]	disp_no_data	layer and block information
@param[in]	line	the line terminal point(start_pos.StartX, start_pos.StartY, end_pos.StartX, end_pos.StartY)
@param[in]	Thickness	the line width
@retval		FJ_ERR_OK				Normal End
@retval		FJ_ERR_OSD_LAYER		Abnormal End. bDisplayID is illegal.
@retval		FJ_ERR_OSD_SEQUENCE		Abnormal End. Icon, font, string or CLUT data has not been load.
*/
extern FJ_ERR_CODE BF_Osd_Shape_Comm_drawslant(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, const FJ_LINE_COORDINATES* line, USHORT Thickness);


#endif // _OSD_SHAPE_COMM_H_
