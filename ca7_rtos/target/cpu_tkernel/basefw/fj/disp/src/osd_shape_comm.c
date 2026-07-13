/**
 * @file		osd_shape_comm.c
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_COMM_INIT"
//#pragma arm section rodata="BASEFW_COMM_CONST"
//#pragma arm section code="BASEFW_COMM_CODE"

#include "fj_custom.h"	// enable BF_Debug_Print
#include "osd_shape_comm.h"
#include "osd_icon_comm.h"
#include "osd_attr.h"
#include "sdram_map_cache_define.h"
#include <math.h>

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define D_OSD_SHAPE_COMM_CIRCLE_SIZE_1	(2)
#define D_OSD_SHAPE_COMM_CIRCLE_SIZE_2	(10)
#define D_OSD_SHAPE_COMM_CIRCLE_SIZE_3	(24)
#define D_OSD_SHAPE_COMM_CIRCLE_SIZE_4	(34)
#define D_OSD_SHAPE_COMM_CIRCLE_SIZE_5	(42)


/*----------------------------------------------------------------------*/
/* Enumeration															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
typedef struct {
	SHORT draw_startX;	/**< Coordinate of X */
	SHORT draw_width;	/**< data's width */
} T_REPEAT_DRAW;
/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
//Cirle' R=42/2
static T_REPEAT_DRAW gOsd_Shape_Comm_Circle_Size_42 [D_OSD_SHAPE_COMM_CIRCLE_SIZE_5/2] = {
	{19,4},
	{14,14},
	{12,18},
	{10,22},
	{9,24},
	{7,28},
	{6,30},
	{5,32},
	{5,32},
	{4,34},
	{3,36},
	{3,36},
	{2,38},
	{2,38},
	{1,40},
	{1,40},
	{1,40},
	{1,40},
	{1,40},
	{0,42},
	{0,42},
};

//Cirle' R=34/2
static T_REPEAT_DRAW gOsd_Shape_Comm_Circle_Size_34 [D_OSD_SHAPE_COMM_CIRCLE_SIZE_4/2] = {
	{15,4},
	{10,14},
	{9,16},
	{7,20},
	{6,22},
	{5,24},
	{4,26},
	{3,28},
	{3,28},
	{2,30},
	{1,32},
	{1,32},
	{1,32},
	{1,32},
	{1,32},
	{0,34},
	{0,34},
};

//Cirle' R=24/2
static T_REPEAT_DRAW gOsd_Shape_Comm_Circle_Size_24 [D_OSD_SHAPE_COMM_CIRCLE_SIZE_3/2] = {
	{10,4},
	{7,10},
	{5,14},
	{4,16},
	{3,18},
	{2,20},
	{2,20},
	{1,22},
	{1,22},
	{0,24},
	{0,24},
	{0,24},
};

//Cirle' R=10/2
static T_REPEAT_DRAW gOsd_Shape_Comm_Circle_Size_10 [D_OSD_SHAPE_COMM_CIRCLE_SIZE_2/2] = {
	{4,2},
	{2,6},
	{1,8},
	{1,8},
	{0,10},
};

//Cirle' R=2/1
static T_REPEAT_DRAW gOsd_Shape_Comm_Circle_Size_02 [D_OSD_SHAPE_COMM_CIRCLE_SIZE_1/2] = {
	{0,2},
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
///////////////////////////////////////////////////
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_COMM_CODE"
/**
draw rectangles which the part of round circle rectangle
@param[in]	wType		round circle rectangle type
@param[in]	global_width		global width
@param[in]	draw_buffer_top_address		Draw buffer's top address
@param[in]	round_data_11_parts		object window position data
*/
static VOID osd_shape_comm_draw_round_part_rectangles_gr(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no, 
							E_FJ_OSD_SHAPE_TYPE wType, USHORT global_width, ULONG draw_buffer_top_address,
							const T_BF_OSD_SHAPE_COMM_SHAPE_INFO round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_END])
{
	UINT32	round_frame_count;
	ULONG	osd_layer_address;
	T_IconColor	Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_END];
#if FJ_OSD_COMM_RGBAXXXX == 4
	Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word = gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.outline_color.word;
	Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY].word    = gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.body_color.word;
#else
	Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.outline_color.word);
	Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY].word    = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.body_color.word);
#endif

	for (round_frame_count = E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_TOP_FRAME; round_frame_count <= E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_RIGHT_FRAME; round_frame_count++) {
		osd_layer_address = (draw_buffer_top_address + (round_data_11_parts[round_frame_count].Rect.Postion.StartX + round_data_11_parts[round_frame_count].Rect.Postion.StartY * global_width) * FJ_OSD_COMM_RGBAXXXX);

		BF_Osd_Shape_Comm_Draw_Rect_GR(round_data_11_parts[round_frame_count].Rect, global_width,
										Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE], osd_layer_address);
	}
	//Draw body rectangle
	if (wType == E_FJ_OSD_SHAPE_TYPE_FILLED_ROUND_FRAME) {
		//In fact top and bottom body will be draw at corner circle drawing
		osd_layer_address = (draw_buffer_top_address + (round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_CENCER].Rect.Postion.StartX + round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_CENCER].Rect.Postion.StartY * global_width) * FJ_OSD_COMM_RGBAXXXX);

		BF_Osd_Shape_Comm_Draw_Rect_GR(round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_CENCER].Rect, global_width,
										Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY], osd_layer_address);
	}
}

/**
set RGBA data to GR display buffer
@param[in/out]	write_address	Address to set
@param[in]	write_data	Data to set
@param[in]	write_size	data num
*/
static VOID osd_shape_comm_set_gr_rbga(OSD_USHORT_INT* write_address, OSD_USHORT_INT write_data, USHORT write_size)
{
	USHORT write_byte_loop_count;
	for (write_byte_loop_count = 0; write_byte_loop_count < write_size; write_byte_loop_count++) {
		*write_address = write_data;
		write_address++;
	}
}

/**
set RGBA data to GR display buffer
@param[in/out]	write_address	Address to set
@param[in]	write_data	Data to set
@param[in]	write_size	data num
@param[in]	alpha	alpha blend
*/
static VOID osd_shape_comm_set_gr_rbga_blend(OSD_USHORT_INT* write_address, OSD_USHORT_INT write_data, USHORT write_size, BYTE alpha)
{
	INT32 write_byte_loop_count;

	for (write_byte_loop_count = write_size; write_byte_loop_count > 0; write_byte_loop_count--) {
		//The checking is finished if write_data's alpha is not 0 on calling.
		BF_Osd_Draw_Comm_Blend_Rgba(write_data, write_address, alpha);
		write_address++;
	}
}

/**
draw ellipse's quadrant data
@param[in]	disp_block_no	common block number
@param[in]	Y	Y
@param[in]	outline_size_y	screen's Y size
@param[in]	ring_type	object what is drwn .line outside/line inside
@param[in]	loop_Start_End	loop start and end in X
@param[in]	Write_infor	draw shape information
@param[in]	d	times in mirror side
@param[in/out]	p_circle_frame_buffer	buffer where draw the data of line
@return		plused Y's num of inside ring
*/
static USHORT osd_shape_comm_set_ring_data(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no, 
										INT32 Y, INT32 outline_size_y, E_BF_OSD_SHAPE_COMM_SHAPE_PARTS ring_type,
										const INT32 loop_Start_End[2], T_BF_OSD_DRAW_COMM_DRAW_SHAPE_INFO Write_infor,
										INT32 d, T_BF_OSD_SHAPE_COMM_RING** p_circle_frame_buffer)
{
	INT32	mirror_dis;
	USHORT	ret = 0;

	if (ring_type == E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE) {
		if (Y < outline_size_y) {
			mirror_dis = d * Write_infor.write_data.global_size;

			// VGR/HGR ch
			OSD_USHORT_INT*	write_address;
			INT32 write_byte_loop_end;

			write_byte_loop_end = (loop_Start_End[1] - loop_Start_End[0]);
			write_address       = (OSD_USHORT_INT*)Write_infor.write_data.osd_layer_address + (mirror_dis + loop_Start_End[0]);
		#if FJ_OSD_COMM_RGBAXXXX == 4
			OSD_USHORT_INT color_outline = gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.outline_color.word;
		#else
			OSD_USHORT_INT color_outline = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.outline_color.word);
		#endif
			osd_shape_comm_set_gr_rbga(write_address, color_outline, write_byte_loop_end);
		}
		else {
			//BF_Debug_Print_Information(("%X,%X,Circle_Start1=,%d,Circle_End2=,%d\n",*p_circle_frame_buffer,((*p_circle_frame_buffer)+d),loop_Start_End[0],loop_Start_End[1]));
			((*p_circle_frame_buffer) + d)->Circle_Start1 = loop_Start_End[0];
			((*p_circle_frame_buffer) + d)->Circle_End2 = loop_Start_End[1];
			(*p_circle_frame_buffer)++;
		}
	}
	else {
		//BF_Debug_Print_Information(("%X,%X,Circle_End1=,%d,Circle_Start2=,%d\n",*p_circle_frame_buffer,((*p_circle_frame_buffer)+d),loop_Start_End[0],loop_Start_End[1]));
		((*p_circle_frame_buffer) + d)->Circle_End1 = loop_Start_End[0];
		((*p_circle_frame_buffer) + d)->Circle_Start2 = (loop_Start_End[1]);
		(*p_circle_frame_buffer)++;
		ret = 1;
	}
	return ret;
}

/**
draw ellipse
@param[in]	disp_block_no	common block number
@param[in]	aRect			The rectangle outside of the ellipse
@param[in]	rang			set draw where of the part of ellipse
@param[in]	Write_infor		draw shape information
@param[in/out]	p_circle_frame_buffer	buffer where draw the data of line
@param[in]	ring_type	shape parts
@param[in]	outline_size_y	size y
@return		plused Y's num of inside ring
*/
static USHORT osd_shape_comm_shape_ellipse(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no, 
											T_BF_OSD_SHAPE_COMM_SHAPE_INFO aRect, BYTE rang, T_BF_OSD_DRAW_COMM_DRAW_SHAPE_INFO Write_infor,
											T_BF_OSD_SHAPE_COMM_RING* p_circle_frame_buffer, E_BF_OSD_SHAPE_COMM_SHAPE_PARTS ring_type, INT32 outline_size_y)
{
	USHORT	ret = 0;
	INT32	X;
	INT32	Y = 0;
	INT32	Yend;
	INT32	E, dEx, dEy, iEx, iEy;
	INT32	A;
	INT32	B = aRect.Rect.Height;
	INT32	A2;
	INT32	B2;
	INT32	x, y;
	INT32	loop_Start_End[2];
	INT32	d;
	T_BF_OSD_SHAPE_COMM_RING* save_circle_frame_buffer;

	A   = aRect.Rect.Width;
	B   = aRect.Rect.Height;
	A2  = A * A;
	B2  = B * B;

	X    = (A >> 1);
	x    = (A & 1) ? 0 : 1;
	y    = B - 1;
	E    = B2 * x * x + A2 * (y * y - B2);
	dEx  = ( B2 << 2) * (x + 1);
	dEy  = (A2 << 2) * (1 - y);
	iEx  = (B2 << 3);	// (correspond to x += 2)  mean 4B2(x+1)
	iEy  = (A2 << 3);	// (correspond to y += 2)  mean 4B2(y+1)
	Yend = ((B ) >> 1);

	do {
		while (E + dEx <= 0) {   /* remain inside of ellipse even if increment X */
			/* increment X */
			E   += dEx;
			dEx += iEx;
			++X;
		}

		/* increment Y */
		d = (B - 1) - (Y << 1);
		loop_Start_End[0] = aRect.Rect.Width - X - 1;
		loop_Start_End[1] = X + 1;

		switch (rang) {
			case E_BF_OSD_SHAPE_COMM_QUADRANT_ONE:
				ret += osd_shape_comm_set_ring_data(disp_block_no, Y, outline_size_y, ring_type, loop_Start_End, Write_infor, 0, &p_circle_frame_buffer);
				break;

			case E_BF_OSD_SHAPE_COMM_QUADRANT_TWO:
				ret += osd_shape_comm_set_ring_data(disp_block_no, Y, outline_size_y, ring_type, loop_Start_End, Write_infor, 0, &p_circle_frame_buffer);
				break;

			case E_BF_OSD_SHAPE_COMM_QUADRANT_THREE:
				ret += osd_shape_comm_set_ring_data(disp_block_no, Y, outline_size_y, ring_type, loop_Start_End, Write_infor, d, &p_circle_frame_buffer);
				break;

			case E_BF_OSD_SHAPE_COMM_QUADRANT_FOUR:
				ret += osd_shape_comm_set_ring_data(disp_block_no, Y, outline_size_y, ring_type, loop_Start_End, Write_infor, d, &p_circle_frame_buffer);
				break;

			case E_BF_OSD_SHAPE_COMM_QUADRANT_ALL:
				// Draw 1 and 2 quadrant
				save_circle_frame_buffer = p_circle_frame_buffer;
				ret += osd_shape_comm_set_ring_data(disp_block_no, Y, outline_size_y, ring_type, loop_Start_End, Write_infor, 0, &save_circle_frame_buffer);
				// Draw 3 and 4 quadrant
				ret += osd_shape_comm_set_ring_data(disp_block_no, Y, outline_size_y, ring_type, loop_Start_End, Write_infor, d, &p_circle_frame_buffer);
				break;

			default:
				// DO NOTHING
				break;
		}

		E   += dEy;
		dEy += iEy;

		// VGR ch
		Write_infor.write_data.osd_layer_address += Write_infor.write_data.global_size * FJ_OSD_COMM_RGBAXXXX;
	} while (++Y < Yend);
	return ret;
}

/**
make ring data
@param[in]	disp_no_data	layer and block information
@param[in]	rect_data	rectangle of the circle data
@param[in]	quadrant	quadrant of cordination
@param[in]	draw_buffer_top_address		data buffer
@param[in]	frame_size		data buffer
@param[out]	circle_frame_buffer		data buffer
@return		count plused to circle_frame_buffer
*/
static USHORT osd_shape_comm_make_ring_data(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, T_BF_OSD_SHAPE_COMM_SHAPE_INFO rect_data, E_BF_OSD_SHAPE_COMM_QUADRANT quadrant,
											ULONG draw_buffer_top_address, T_POS const* const frame_size, T_BF_OSD_SHAPE_COMM_RING* circle_frame_buffer)
{
	T_BF_OSD_DRAW_COMM_DRAW_SHAPE_INFO Write_infor;

	Write_infor.write_data.x = rect_data.Rect.Postion.StartX;
	Write_infor.write_data.y = rect_data.Rect.Postion.StartY;
	Write_infor.drwa_layer = *disp_no_data;
	USHORT screen_size[2];//Screen size.[0]: width [1]:height

	BF_Osd_Attr_Get_Draw_Area_Size(disp_no_data,  screen_size);
	Write_infor.write_data.osd_layer_address = (UCHAR*)(draw_buffer_top_address + (Write_infor.write_data.x + screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] * Write_infor.write_data.y) * FJ_OSD_COMM_RGBAXXXX);

	Write_infor.write_data.global_size =
	Write_infor.write_data.line_feed = screen_size[D_BF_OSD_ATTR_INDEX_WIDTH];

	osd_shape_comm_shape_ellipse(disp_no_data->disp_block_no, rect_data, quadrant, Write_infor, circle_frame_buffer, E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE, frame_size->StartY);

	rect_data.Rect.Postion.StartX += (frame_size->StartX);
	rect_data.Rect.Postion.StartY += (frame_size->StartY);
	rect_data.Rect.Width -= ((frame_size->StartX) << 1);
	rect_data.Rect.Height -= (frame_size->StartY << 1);

	return osd_shape_comm_shape_ellipse(disp_no_data->disp_block_no, rect_data, quadrant, Write_infor, circle_frame_buffer, E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY, frame_size->StartY);
}

/**
 *	@brief set color from left corner to right corner
 *	@param  very_thin_frame: the flag if frame size is 1 pixel
 *	        round_rect: round rectangle data(IN)
 *			round_corner_data: round rectangle's left and right corner data (IN/OUT)
 *			oc_Write_infor:write color date area
 *	@return none
 *	@note	none
 *	@attention	None
 */
static USHORT osd_shape_comm_set_corner_color(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, T_BF_OSD_SHAPE_COMM_SHAPE_INFO const* const round_rect,
											  ULONG draw_buffer_top_address, T_POS const* const frame_size, T_BF_OSD_SHAPE_COMM_ROUND_CORNER* round_corner_data_ud)
{
	USHORT count_circle_frame_buffer = 0;

	osd_shape_comm_make_ring_data(disp_no_data, round_corner_data_ud->round_corner_data[E_BF_OSD_SHAPE_COMM_QUADRANT_TWO],
						E_BF_OSD_SHAPE_COMM_QUADRANT_TWO, draw_buffer_top_address, frame_size, round_corner_data_ud->circle_frame_buffer_left);

	// Set right-top corner circle
	count_circle_frame_buffer = osd_shape_comm_make_ring_data(disp_no_data, round_corner_data_ud->round_corner_data[E_BF_OSD_SHAPE_COMM_QUADRANT_THREE],
						E_BF_OSD_SHAPE_COMM_QUADRANT_THREE, draw_buffer_top_address, frame_size, round_corner_data_ud->circle_frame_buffer_left);

	count_circle_frame_buffer = osd_shape_comm_make_ring_data(disp_no_data, round_corner_data_ud->round_corner_data[E_BF_OSD_SHAPE_COMM_QUADRANT_ONE],
						E_BF_OSD_SHAPE_COMM_QUADRANT_ONE, draw_buffer_top_address, frame_size, round_corner_data_ud->circle_frame_buffer_right);

	count_circle_frame_buffer = osd_shape_comm_make_ring_data(disp_no_data, round_corner_data_ud->round_corner_data[E_BF_OSD_SHAPE_COMM_QUADRANT_FOUR],
						E_BF_OSD_SHAPE_COMM_QUADRANT_FOUR, draw_buffer_top_address, frame_size, round_corner_data_ud->circle_frame_buffer_right);

	return count_circle_frame_buffer;
}

/**
 *	@brief  set 4 corner color to display memory
 *	@param  down_side_y: the Y distance between top corner and bottom corner(IN)
 *			count_circle_frame_buffer: circle_frame_buffer's size (IN)
 *			circle_frame_buffer: corner data(IN/OUT)
 *			oc_Write_infor:write color date area(IN/OUT)
 *	@return none
 *	@note	none
 *	@attention	None
 */
static VOID osd_shape_comm_set_4_corner_color_gr(SHORT frame_width, T_BF_OSD_SHAPE_COMM_SHAPE_INFO const* const round_rect, USHORT count_circle_frame_buffer,
													T_BF_OSD_SHAPE_COMM_ROUND_CORNER* round_corner_circle, T_BF_OSD_DRAW_COMM_DRAW_SHAPE_INFO* Write_infor,
													 const UINT32 color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_END])
{
	USHORT i, write_byte_loop_end;
	OSD_USHORT_INT* write_address;
	INT32 translate_2_round_rect;

	translate_2_round_rect = (round_rect->Rect.Width - round_corner_circle->round_corner_data[E_BF_OSD_SHAPE_COMM_QUADRANT_ONE].Rect.Width);

	// Draw Y data
	for (i = 0; i < count_circle_frame_buffer; i++) {
		// left up corner ring
		round_corner_circle->circle_frame_buffer_left[i].Circle_End1    += frame_width;
		round_corner_circle->circle_frame_buffer_right[i].Circle_Start2 += (frame_width + translate_2_round_rect);
		round_corner_circle->circle_frame_buffer_right[i].Circle_End2   += ( translate_2_round_rect);

		// Draw up parts

		// Write left part
		write_byte_loop_end = (round_corner_circle->circle_frame_buffer_left[i].Circle_End1 - round_corner_circle->circle_frame_buffer_left[i].Circle_Start1);
		write_address = (OSD_USHORT_INT*)Write_infor->write_data.osd_layer_address + round_corner_circle->circle_frame_buffer_left[i].Circle_Start1;
		osd_shape_comm_set_gr_rbga(write_address, (OSD_USHORT_INT)color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE], write_byte_loop_end);

		// Write right part
		write_byte_loop_end = (round_corner_circle->circle_frame_buffer_right[i].Circle_End2 - round_corner_circle->circle_frame_buffer_right[i].Circle_Start2);
		write_address = (OSD_USHORT_INT*)Write_infor->write_data.osd_layer_address + round_corner_circle->circle_frame_buffer_right[i].Circle_Start2;
		osd_shape_comm_set_gr_rbga(write_address, (OSD_USHORT_INT)color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE], write_byte_loop_end);

		if (round_rect->wType == E_FJ_OSD_SHAPE_TYPE_FILLED_ROUND_FRAME) {
			// set body color
			write_byte_loop_end = (round_corner_circle->circle_frame_buffer_right[i].Circle_Start2 - round_corner_circle->circle_frame_buffer_left[i].Circle_End1);
			write_address = (OSD_USHORT_INT*)Write_infor->write_data.osd_layer_address + round_corner_circle->circle_frame_buffer_left[i].Circle_End1;
			osd_shape_comm_set_gr_rbga(write_address, (OSD_USHORT_INT)color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY], write_byte_loop_end);
		}
		Write_infor->write_data.osd_layer_address += (Write_infor->write_data.global_size * FJ_OSD_COMM_RGBAXXXX);
	}
}

/**
 *	@brief  set 4 corner color to display memory
 *	@param  down_side_y: the Y distance between top corner and bottom corner(IN)
 *			count_circle_frame_buffer: circle_frame_buffer's size (IN)
 *			circle_frame_buffer: corner data(IN/OUT)
 *			oc_Write_infor:write color date area(IN/OUT)
 *	@return none
 *	@note	none
 *	@attention	None
 */
static VOID osd_shape_comm_set_4_corner_color(SHORT frame_width, T_BF_OSD_SHAPE_COMM_SHAPE_INFO const* const round_rect, USHORT count_circle_frame_buffer,
												T_BF_OSD_SHAPE_COMM_ROUND_CORNER* round_corner_circle, T_BF_OSD_DRAW_COMM_DRAW_SHAPE_INFO* Write_infor,
												 const UINT32 color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_END])
{
	osd_shape_comm_set_4_corner_color_gr(frame_width, round_rect, count_circle_frame_buffer, round_corner_circle, Write_infor, color_yuv);
}

/**
 *	@brief Draw 7 parts(4 circles and 3 rectangle) of round rectangle
 *	@param  very_thin_frame: the flag if frame size is 1 pixel
 *	        round_rect round rectangle's data(IN)
 *			round_data_11_parts(IN)
 *			oc_Write_infor :Draw address
 *			draw_buffer_top_address:Draw address's top
 *	@return none
 *	@note	none
 *	@attention	None
 */
static VOID osd_shape_comm_draw_round_rect(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no,T_BF_OSD_SHAPE_COMM_SHAPE_INFO const* const round_rect,
										   const T_BF_OSD_SHAPE_COMM_SHAPE_INFO round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_END],
										   ULONG draw_buffer_top_address, T_POS const* const frame_size, T_BF_OSD_DRAW_COMM_DRAW_SHAPE_INFO* Write_infor)
{
	T_BF_OSD_SHAPE_COMM_ROUND_CORNER round_corner_circle;
	UINT32	color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_END];
	USHORT	valid_data_num;
	USHORT	count_circle_frame_buffer;
	ULONG	work_buffer;

	valid_data_num = ((round_data_11_parts[E_BF_OSD_SHAPE_COMM_QUADRANT_TWO].Rect.Height - (frame_size->StartY << 1)) >> 1);

	// use display area as working area because the area will be reset after SETPARM
	work_buffer = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(Sdram_Map_Osd_Get_WORK_Addr());

	round_corner_circle.circle_frame_buffer_left = (T_BF_OSD_SHAPE_COMM_RING*)work_buffer;
	round_corner_circle.circle_frame_buffer_right = round_corner_circle.circle_frame_buffer_left + (round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_TOP].Rect.Height + 1);

	// divide the roud to following
	//++---+------------1.1--------------+---++
	//++---+-----------------------------+---++
	//||3  |                             | 3 ||
	//++---+*****************************+---++
	//||   |                             |   ||
	//||2.1|           2.3               |2.2||
	//++---+*****************************+---++
	//||4  |                             | 4 ||
	//++---+-----------------------------+---++
	//++---+------------1.2--------------+---++
	memset(round_corner_circle.circle_frame_buffer_left, 0, (round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_TOP].Rect.Height + 1));
	memset(round_corner_circle.circle_frame_buffer_right, 0, (round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_TOP].Rect.Height + 1));

	round_corner_circle.round_corner_data[E_BF_OSD_SHAPE_COMM_QUADRANT_TWO]   = round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_TOP];
	round_corner_circle.round_corner_data[E_BF_OSD_SHAPE_COMM_QUADRANT_THREE] = round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_BOTTOM];
	round_corner_circle.round_corner_data[E_BF_OSD_SHAPE_COMM_QUADRANT_ONE]   = round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_TOP];
	round_corner_circle.round_corner_data[E_BF_OSD_SHAPE_COMM_QUADRANT_FOUR]  = round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_BOTTOM];

	// draw 4 conner circle
	count_circle_frame_buffer = osd_shape_comm_set_corner_color(&(Write_infor->drwa_layer), round_rect, draw_buffer_top_address, frame_size, &round_corner_circle);

#if FJ_OSD_COMM_RGBAXXXX == 4
	color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE] = gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.outline_color.word;
	color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY]    = gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.body_color.word;
#else
	color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE]= BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.outline_color.word);
	color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY]    = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.body_color.word);
#endif

	osd_shape_comm_draw_round_part_rectangles_gr(disp_block_no, round_rect->wType, Write_infor->write_data.global_size, draw_buffer_top_address, round_data_11_parts);
	Write_infor->write_data.osd_layer_address += (frame_size->StartY * Write_infor->write_data.global_size * FJ_OSD_COMM_RGBAXXXX);
	osd_shape_comm_set_4_corner_color(frame_size->StartX, round_rect, valid_data_num, &round_corner_circle, Write_infor, color_yuv);

	// set color between bottom-left and right corner
	Write_infor->write_data.osd_layer_address += (round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_CENCER].Rect.Height * (Write_infor->write_data.global_size * FJ_OSD_COMM_RGBAXXXX));
	round_corner_circle.circle_frame_buffer_right += (count_circle_frame_buffer);
	round_corner_circle.circle_frame_buffer_left  += (count_circle_frame_buffer);

	osd_shape_comm_set_4_corner_color(frame_size->StartX, round_rect, valid_data_num, (&round_corner_circle), Write_infor, color_yuv);
}

/**
 *	@brief Draw  get draw buffer address 
 *	@param 	global_width : display's global width(I)
 *			drawing_postion: Position of drawing shape
 *			Write_infor:Firstly drawing address when round rectangle is drawn.
 *						Real drawing address when circle is drawn.(O)
 *			draw_buffer_top_address: real drawing address(O)
 *	@return drawing buffer address
 *	@note	none
 *	@attention	none
 */
static ULONG Osd_Draw_Comm_Get_GR_DrawIng_Address(const T_POS* drawing_postion, T_BF_OSD_DRAW_COMM_DRAW_SHAPE_INFO* Write_infor)
{
	ULONG draw_buffer_top_address;//draw buffer address;
	ULONG osd_layer_address;//Drawing address

	BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(drawing_postion, &osd_layer_address, &(Write_infor->drwa_layer));

	Write_infor->write_data.osd_layer_address = (BYTE*)osd_layer_address;

	draw_buffer_top_address = osd_layer_address - ( drawing_postion->StartX + Write_infor->write_data.global_size * drawing_postion->StartY) * FJ_OSD_COMM_RGBAXXXX; //Top address is real address
	return draw_buffer_top_address;
}

/**
 *	@brief Draw  rectangles by rectangles array
 *	@param vgr_ch: VGR ch No. Don't care if it's OC
 *			draw_buffer_top_address : drawing address when round rectangle is drawn
 *			dispWnd : display window(I)
 *			Write_infor:Firstly drawing address when round rectangle is drawn.
 *						Real drawing address when circle is drawn.(O)
 *			draw_buffer_top_address: real drawing address(O)
 *	@return none
 *	@note	none
 *	@attention	none
 */
static BOOL osd_shape_comm_set_draw_buffer_addr(T_Rect const* const dispWnd, T_BF_OSD_SHAPE_COMM_SHAPE_INFO* rect_data,
												T_BF_OSD_DRAW_COMM_DRAW_SHAPE_INFO* Write_infor, ULONG* draw_buffer_top_address)
{
	BOOL is_patial_draw = FALSE;

	if ((dispWnd->Postion.StartX == rect_data->Rect.Postion.StartX) && (dispWnd->Postion.StartY == rect_data->Rect.Postion.StartY) &&
		(dispWnd->Width == rect_data->Rect.Width) && (dispWnd->Height == rect_data->Rect.Height) &&
		(rect_data->Rect.Postion.StartX >= 0) && (rect_data->Rect.Postion.StartY >= 0)) {
		// obj is same as dis_wnd
		Write_infor->write_data.x = rect_data->Rect.Postion.StartX;
		Write_infor->write_data.y = rect_data->Rect.Postion.StartY;

		//HGR/VGR0/VGR1/every softlayer
		*draw_buffer_top_address = Osd_Draw_Comm_Get_GR_DrawIng_Address(&rect_data->Rect.Postion, Write_infor);
	}
	else {
		// Draw at (0,0) on work area
		Write_infor->write_data.x =
		Write_infor->write_data.y =
		rect_data->Rect.Postion.StartX =
		rect_data->Rect.Postion.StartY = 0;

		is_patial_draw = TRUE;

		// Use next buffer as work area
		*draw_buffer_top_address = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(Sdram_Map_Osd_Get_OSD_Next_Buffer(&(Write_infor->drwa_layer)));
		Write_infor->write_data.osd_layer_address = (BYTE*)(*draw_buffer_top_address);
	}
	return is_patial_draw;
}

/**
 *	@brief Set circle inside data 
 *	@param  very_thin_frame: the flag if frame size is 1 pixel
 *	        frame_size: frame thickness(I)
 *			color_yuv : color data (RGBA) or palatte OS_USER_ID when it's OC(I)
 *			rect_data : object window position data(I)
 *			Write_infor:Write address information(I/O)
 *			circle_frame_buffer: write position(I/O)
 *	@return none
 *	@note	none
 *	@attention	none
 */
static VOID osd_shape_comm_set_circle_inside_color_data(T_POS const* const frame_size, const UINT32 color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_END],
													INT32 count_circle_frame_buffer, E_FJ_OSD_SHAPE_TYPE wType, INT32 translate_2_round_rect,
													T_BF_OSD_DRAW_COMM_DRAW_SHAPE_INFO* Write_infor, T_BF_OSD_SHAPE_COMM_RING* circle_frame_buffer)
{
	INT32 i;

	for (i = 0; i < count_circle_frame_buffer; i++) {	// inside ring' loop
		circle_frame_buffer[i].Circle_End1   += frame_size->StartX;
		circle_frame_buffer[i].Circle_Start2 += (frame_size->StartX + translate_2_round_rect);
		circle_frame_buffer[i].Circle_End2   += (translate_2_round_rect);

		OSD_USHORT_INT* write_address;
		USHORT write_byte_loop_end;

		write_byte_loop_end = (circle_frame_buffer[i].Circle_End1 - circle_frame_buffer[i].Circle_Start1);

		// Draw left ring parts
		write_address = (OSD_USHORT_INT*)Write_infor->write_data.osd_layer_address + circle_frame_buffer[i].Circle_Start1;
		osd_shape_comm_set_gr_rbga(write_address, (OSD_USHORT_INT)color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE], write_byte_loop_end);

		// Draw right ring parts
		write_byte_loop_end = (circle_frame_buffer[i].Circle_End2 - circle_frame_buffer[i].Circle_Start2);
		write_address = (OSD_USHORT_INT*)Write_infor->write_data.osd_layer_address + circle_frame_buffer[i].Circle_Start2;
		osd_shape_comm_set_gr_rbga(write_address, (OSD_USHORT_INT)color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE], write_byte_loop_end);

		if (wType == E_FJ_OSD_SHAPE_TYPE_FILLED_CIRCLE) {
			// Draw body parts
			write_byte_loop_end = (circle_frame_buffer[i].Circle_Start2 - circle_frame_buffer[i].Circle_End1);
			write_address = (OSD_USHORT_INT*)Write_infor->write_data.osd_layer_address + circle_frame_buffer[i].Circle_End1;
			osd_shape_comm_set_gr_rbga(write_address, (OSD_USHORT_INT)color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY], write_byte_loop_end);
		}
		Write_infor->write_data.osd_layer_address += (Write_infor->write_data.global_size * FJ_OSD_COMM_RGBAXXXX);
	}
}

/**
 *	@brief Draw  rectangles by rectangles array
 *	@param  very_thin_frame: the flag if frame size is 1 pixel
 *			draw_buffer_top_address : drawing address when round rectangle is drawn
 *			color_yuv : frame and body color
 *			rect_data: display window (I/O)
 *			frame_size: display_data's size(I)
 *			Write_infor:Firstly drawing address when round rectangle is drawn.
 *						Real drawing address when circle is drawn.
 *	@return none
 *	@note	none
 *	@attention	none
 */
static FJ_ERR_CODE osd_shape_comm_draw_circle_round_rect(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no, 
														 ULONG draw_buffer_top_address,
														 T_BF_OSD_SHAPE_COMM_SHAPE_INFO* rect_data,
														 T_POS const* const frame_size,
														 T_BF_OSD_DRAW_COMM_DRAW_SHAPE_INFO* Write_infor)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	ULONG work_buffer;

	T_BF_OSD_SHAPE_COMM_RING* circle_frame_buffer;
	UINT32 color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_END];

#if FJ_OSD_COMM_RGBAXXXX == 4
	color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE] = gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.outline_color.word;
	color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY]    = gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.body_color.word;
#else
	color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE] = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.outline_color.word);
	color_yuv[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY]    = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.body_color.word);
#endif

	switch (rect_data->wType) {
		case E_FJ_OSD_SHAPE_TYPE_ROUND_FRAME:			// FALL THROUGH
		case E_FJ_OSD_SHAPE_TYPE_FILLED_ROUND_FRAME:
		{
			// divide round rect to 11 parts
			T_BF_OSD_SHAPE_COMM_SHAPE_INFO round_data[E_BF_OSD_SHAPE_COMM_ROUND_POS_END];
			// thickness Y will be ignoed
			ret = BF_Osd_Shape_Comm_Divide_Round_11_Parts(disp_block_no, rect_data, round_data, frame_size);
			if (ret == FJ_ERR_OK) {
				osd_shape_comm_draw_round_rect(disp_block_no, rect_data, round_data, draw_buffer_top_address, frame_size, Write_infor);
			}
			break;
		}

		case E_FJ_OSD_SHAPE_TYPE_CIRCLE:				// FALL THROUGH
		case E_FJ_OSD_SHAPE_TYPE_FILLED_CIRCLE:
		{
			INT32 circle_frame_buffer_num;

			//thickness Y will be ignoed
			//use display area as working area because the area will be reset after SETPARM
			work_buffer         = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(Sdram_Map_Osd_Get_WORK_Addr());
			circle_frame_buffer = (T_BF_OSD_SHAPE_COMM_RING*)work_buffer;
			memset(circle_frame_buffer, 0, PIX_SIZ_QVGA); //The size PIX_SIZ_QVGA is enougth fo rT_Ring

			circle_frame_buffer_num = osd_shape_comm_shape_ellipse(disp_block_no, *rect_data, E_BF_OSD_SHAPE_COMM_QUADRANT_ALL, *Write_infor, circle_frame_buffer,
																	E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE, frame_size->StartY);

			rect_data->Rect.Postion.StartX += (frame_size->StartX);
			rect_data->Rect.Postion.StartY += frame_size->StartY;
			rect_data->Rect.Width -= ((frame_size->StartX) << 1);
			rect_data->Rect.Height -= (frame_size->StartY << 1);

			circle_frame_buffer_num = osd_shape_comm_shape_ellipse(disp_block_no, *rect_data, E_BF_OSD_SHAPE_COMM_QUADRANT_ALL, *Write_infor, circle_frame_buffer,
																   E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY, frame_size->StartY);
			// set color ring
			Write_infor->write_data.osd_layer_address += (Write_infor->write_data.global_size * frame_size->StartY * FJ_OSD_COMM_RGBAXXXX);
			osd_shape_comm_set_circle_inside_color_data(frame_size, color_yuv, circle_frame_buffer_num, rect_data->wType, 0, Write_infor, circle_frame_buffer);
			break;
		}

		default:
			// DO NOTHING
			break;
	}
	return ret;
}

/**
 *	@brief Draw  rectangles by rectangles array
 *	@param bdisplayid : VGR0/VGR1/HGR or softlayer
 *			screen_size : ch display size. [0]: width [1]:height
 *			draw_wnd:
 *			dispWnd: display window (I/O)
 *			display_data: draw rectangle data(I/O)
 *			display_data_size: display_data's size
 *	@return none
 *	@note	none
 *	@attention	none
 */
static VOID osd_shape_comm_5_parts_rect_gr(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, const USHORT screen_size[2], T_Rect const* const dispWnd,
											 T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO* display_data, SHORT display_data_size)
{
	UINT32	i;
	ULONG	gr_write_infor;
	T_Rect	draw_wnd;

	for (i = 0; i < display_data_size; i++) {
		// Use VGR's buffer as draw buffer
		if (BF_Osd_Draw_Comm_Is_Rect_At_LCD_Inside(screen_size, &display_data[i].rect_data.Rect)) {

			if (BF_Osd_Draw_Comm_Get_Overlap_Rect(display_data[i].rect_data.Rect, *dispWnd, &draw_wnd)) {
				// Get VGR write information
				BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(&draw_wnd.Postion, &gr_write_infor, disp_no_data);
				// Make its to VGR buffer
				BF_Osd_Shape_Comm_Draw_Rect_GR(draw_wnd, screen_size[D_BF_OSD_ATTR_INDEX_WIDTH], display_data[i].Color_Blending, gr_write_infor);
			}
		}
	}
}

/**
 *	@brief Draw  rectangles by rectangles array
 *	@param bdisplayid : VGR0/VGR1/HGR or softlayer
 *			screen_size : ch display size. [0]: width [1]:height
 *			draw_wnd:
 *			dispWnd: display window (I/O)
 *			display_data: draw rectangle data(I/O)
 *			display_data_size: display_data's size
 *	@return none
 *	@note	none
 *	@attention	none
 */
static VOID osd_shape_comm_5_parts_rect_gr_blending(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, const USHORT screen_size[2],
											T_Rect const* const dispWnd, T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO* display_data,
											SHORT display_data_size, BYTE alpha)
{
	UINT32	i;
	ULONG	gr_write_infor;
	T_Rect	draw_wnd;

	if(alpha != 0 ){
		for (i = 0; i < display_data_size; i++) {
	#if (FJ_OSD_COMM_RGBAXXXX == 2)
			if((display_data[i].Color_Blending.word & 0x0f) !=0)
	#else
			if((display_data[i].Color_Blending.word & 0xff) !=0)
	#endif
			{
				// Use VGR's buffer as draw buffer
				if (BF_Osd_Draw_Comm_Is_Rect_At_LCD_Inside(screen_size, &display_data[i].rect_data.Rect)) {

					if (BF_Osd_Draw_Comm_Get_Overlap_Rect(display_data[i].rect_data.Rect, *dispWnd, &draw_wnd)) {
						// Get VGR write information
						INT32 line_index,width_index;
						OSD_USHORT_INT* write_address;
						BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(&draw_wnd.Postion, &gr_write_infor, disp_no_data);
						// Make its to VGR buffer
						write_address = (OSD_USHORT_INT*)gr_write_infor;

						for(line_index = draw_wnd.Height; line_index >0; line_index--){
							for(width_index = draw_wnd.Width; width_index >0; width_index--){
								BF_Osd_Draw_Comm_Blend_Rgba((OSD_USHORT_INT)display_data[i].Color_Blending.word, write_address, alpha);
								write_address++;
							}
							write_address -= draw_wnd.Width;
							write_address += screen_size[D_BF_OSD_ATTR_INDEX_WIDTH];
						}
					}
				}
			}
		}
	}
}

/**
 *	@brief Draw a donus shap on osd
 *	@param vgr_ch : og1 or og0
 *			ogsize : VGR ch data
 *			rect_data: drawing shape data
 *			dispWnd: display window
 *			Color_Blending: frame's color and body' color
 *			frame_theckness: frame's size
 *			DrawMode: Draw mode
 *			alpha: Blend rate that is invalid when DrawMode is E_DrawMode_Effect_Optimized
 *	@return 0 : ok -1 : wStartX is not even error
 *					2:E_ERROR_CODE_UWWIDTH_NO_EVEN
 *	@note		The exterior and interior edge size always is 1 pixel.
 *				The exterior and interior edge's Y color is defined by Pen color
 *				The middle part's Y color is defined by Brush color
 *				All drawing part's U/V/A is defined by Brush one
 *	@attention	none
 */
static FJ_ERR_CODE	osd_shape_comm_draw_donut(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, const USHORT screen_size[2],
											 T_BF_OSD_SHAPE_COMM_SHAPE_INFO* rect_data, T_Rect const* const dispWnd,
											 const T_IconColor Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_END],
											 T_POS const* const frame_theckness, E_DrawMode DrawMode, BYTE alpha)
{
	FJ_ERR_CODE		ret = FJ_ERR_OK;
	T_Rect			draw_wnd;
	OSD_USHORT_INT*	gr_write_infor;
	SHORT			i;
	T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_THE_END];

	// Divide the rect to 13 rects
	BF_Osd_Shape_Comm_Divide_Donut_13_Parts(disp_no_data->disp_block_no, rect_data, frame_theckness, display_data);

	// Draw Y data
	for (i = 0; i < E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER; i++) {
		if (BF_Osd_Draw_Comm_Is_Rect_At_LCD_Inside(screen_size, &display_data[i].rect_data.Rect)) {

			if (BF_Osd_Draw_Comm_Get_Overlap_Rect(display_data[i].rect_data.Rect, *dispWnd, &draw_wnd)) {
				ULONG temp_gr_write_infor;

				// Get VGR write information
				BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(&draw_wnd.Postion, &temp_gr_write_infor, disp_no_data);
				gr_write_infor = (OSD_USHORT_INT*)temp_gr_write_infor;

				// Make its to VGR buffer
				INT32 j;
				switch (i) {
					case E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_TOP:		// FALL THROUGH
					case E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_TOP:		// FALL THROUGH
					case E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_BOTTOM:	// FALL THROUGH
					case E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_BOTTOM:
					{
						for( j = gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.Donut_Rect_frame_size; j>0;j--){
							if(DrawMode == E_DrawMode_Speed_Optimized) {
								//Speed draw
								osd_shape_comm_set_gr_rbga(gr_write_infor, (OSD_USHORT_INT)Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word, draw_wnd.Width);
							}
							else{
								//Effect draw
								osd_shape_comm_set_gr_rbga_blend(gr_write_infor, (OSD_USHORT_INT)Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word, draw_wnd.Width,alpha);
							}
							gr_write_infor += screen_size[D_BF_OSD_ATTR_INDEX_WIDTH];
						}

						break;
					}

					case E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_LEFT:		// FALL THROUGH
					case E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_LEFT:		// FALL THROUGH
					case E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_RIGHT:		// FALL THROUGH
					case E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_RIGHT:
					{
						if(DrawMode == E_DrawMode_Speed_Optimized) {
							//Speed draw

							for (j = draw_wnd.Height; j >0 ; j--) {
								osd_shape_comm_set_gr_rbga(gr_write_infor, (OSD_USHORT_INT)Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word, gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.Donut_Rect_frame_size);
								gr_write_infor += screen_size[D_BF_OSD_ATTR_INDEX_WIDTH];
							}
						}
						else{
							//Effect draw
							for (j = draw_wnd.Height; j >0 ; j--) {
								//Effect draw
								osd_shape_comm_set_gr_rbga_blend(gr_write_infor, (OSD_USHORT_INT)Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word, gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.Donut_Rect_frame_size,alpha);
								gr_write_infor += screen_size[D_BF_OSD_ATTR_INDEX_WIDTH];
							}
						}
						break;
					}

					case E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_TOP:			// FALL THROUGH
					case E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_BOTTOM:		// FALL THROUGH
					case E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_LEFT:		// FALL THROUGH
					case E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_RIGHT:
						if(DrawMode == E_DrawMode_Speed_Optimized) {
							//Speed draw
							BF_Osd_General_Fill_Box_For_GR((OSD_USHORT_INT)Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY].word,
											screen_size[D_BF_OSD_ATTR_INDEX_WIDTH], gr_write_infor, draw_wnd.Width, draw_wnd.Height);
						}
						else{
							//Effect draw
							INT32 line_index, width_index;
							for(line_index = draw_wnd.Height; line_index >0; line_index--){
								for(width_index = draw_wnd.Width; width_index >0; width_index--){
									BF_Osd_Draw_Comm_Blend_Rgba((OSD_USHORT_INT)Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY].word, gr_write_infor, alpha);
									gr_write_infor++;
								}
								gr_write_infor +=(screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] - draw_wnd.Width);
							}
						}
						break;

					default:
						// DO NOTHING
						break;
				}
			}
		}
	}
	return ret;
}

///////////////////////////////////////////////////
//  Following are BASEFW_VIEW_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_VIEW_CODE"
/**
 *	@brief Draw one line of Y
 *	@param in_addr: draw address
 *			in_x:position of X
 *			in_y:position of Y
 *			in_w:width
 *			in_gw: global width
 *			in_value: Y
 *	@return None
 *	@note	None
 *	@attention	None
 */
static VOID osd_shape_comm_draw_2color_frame_horizontal_y(ULONG in_addr, ULONG in_x, ULONG in_y, ULONG in_w, ULONG in_gw, ULONG in_value)
{
	ULONG* pbuf, i;

	pbuf = (ULONG*)(in_addr + in_gw * in_y + in_x);
	in_w >>= 2;
	for ( i = 0; i < in_w; i++) {
		*pbuf++ = in_value;
	}
}

/**
 *	@brief Draw one line of UV
 *	@param in_addr: draw address
 *			in_x:position of X
 *			in_y:position of Y
 *			in_w:width
 *			in_gw: global width
 *			in_value: U or V
 *	@return None
 *	@note	None
 *	@attention	None
 */
static VOID osd_shape_comm_draw_2color_frame_horizontal_cc(ULONG in_addr, ULONG in_x, ULONG in_y, ULONG in_w, ULONG in_gw, UINT32 in_value)
{
	ULONG i;
	UINT32* pbuf;

	pbuf = (UINT32*)(in_addr + in_gw * in_y + in_x);
	in_w >>= 2;
	for ( i = 0; i < in_w; i++) {
		*pbuf++ = in_value;
	}
}

/**
 *	@brief Draw a donut's left and right rectangle's Y.
 *
 *	@param in_addr: draw address
 *			in_x:position of X
 *			in_y:position of Y
 *			in_h:Height
 *			in_gw: global width
 *			in_value: Y
 *	@return None
 *	@note	None
 *	@attention	None
 */
static VOID osd_shape_comm_draw_2color_frame_vertical_y(ULONG in_addr, ULONG in_x, ULONG in_y, ULONG in_h, ULONG in_gw, ULONG in_value)
{
	ULONG* pbuf, i;

	pbuf = (ULONG*)(in_addr + in_gw * in_y + in_x);
	in_gw >>= 2;
	for ( i = 0; i < in_h; i++, pbuf += in_gw ) {
		*pbuf = in_value;
	}
}

/**
 *	@brief Draw a donut's left and right rectangle's UV.
 *
 *	@param in_addr: draw address
 *			in_x:position of X
 *			in_y:position of Y
 *			in_h:Height
 *			in_gw: global width
 *			in_value: U or V
 *	@return None
 *	@note	None
 *	@attention	None
 */
static VOID osd_shape_comm_draw_2color_frame_vertical_cc(ULONG in_addr, ULONG in_x, ULONG in_y, ULONG in_h, ULONG in_gw, UINT32 in_value)
{
	ULONG i;
	UINT32* pbuf;

	pbuf = (UINT32*)(in_addr + in_gw * in_y + in_x);
	in_gw >>= 2;
	for ( i = 0; i < in_h; i++, pbuf += in_gw ) {
		*pbuf = in_value;
	}
}

/**
 *	@brief Draw a donut's Y
 *	@param in_addr: draw address
 *			in_x:position of X
 *			in_y:position of Y
 *			in_w:Width
 *			in_h:Height
 *			in_gw: global width
 *			in_edge: outline's Y
 *			in_main: body's color of Y
 *	@return None
 *	@note		the U/V of outline must be as same as body
 *	@attention	None
 */
static VOID osd_shape_comm_draw_2color_frame_y(ULONG in_addr, ULONG in_x, ULONG in_y, ULONG in_w, ULONG in_h, ULONG in_gw, BYTE in_edge, BYTE in_main)
{
	ULONG Y1Patten;
	ULONG Y2Patten;

	in_edge  = in_edge & 0xff;
	Y1Patten = in_edge << 24 | in_edge << 16 | in_edge << 8 | in_edge;
	Y2Patten = in_main << 24 | in_main << 16 | in_main << 8 | in_main;

	osd_shape_comm_draw_2color_frame_horizontal_y(in_addr, in_x, in_y    , in_w, in_gw, Y1Patten);
	osd_shape_comm_draw_2color_frame_horizontal_y(in_addr, in_x, in_y + 1, in_w, in_gw, Y2Patten);
	osd_shape_comm_draw_2color_frame_horizontal_y(in_addr, in_x, in_y + 2, in_w, in_gw, Y2Patten);
	osd_shape_comm_draw_2color_frame_horizontal_y(in_addr, in_x, in_y + 3, in_w, in_gw, Y1Patten);

	osd_shape_comm_draw_2color_frame_horizontal_y(in_addr, in_x, in_y + in_h - 1, in_w    , in_gw, Y1Patten);
	osd_shape_comm_draw_2color_frame_horizontal_y(in_addr, in_x, in_y + in_h - 2, in_w, in_gw, Y2Patten);
	osd_shape_comm_draw_2color_frame_horizontal_y(in_addr, in_x, in_y + in_h - 3, in_w, in_gw, Y2Patten);
	osd_shape_comm_draw_2color_frame_horizontal_y(in_addr, in_x, in_y + in_h - 4, in_w, in_gw, Y1Patten);

	Y1Patten = in_main << 24 | in_main << 16 | in_main << 8 | in_edge;
	osd_shape_comm_draw_2color_frame_vertical_y(in_addr, in_x, in_y + 1,        2, in_gw, Y1Patten);
	osd_shape_comm_draw_2color_frame_vertical_y(in_addr, in_x, in_y + in_h - 3, 2, in_gw, Y1Patten);

	Y1Patten = in_edge << 24 | in_main << 16 | in_main << 8 | in_main;
	osd_shape_comm_draw_2color_frame_vertical_y(in_addr, in_x + in_w - 4, in_y + 1,        2, in_gw, Y1Patten);
	osd_shape_comm_draw_2color_frame_vertical_y(in_addr, in_x + in_w - 4, in_y + in_h - 3, 2, in_gw, Y1Patten);

	Y1Patten = in_edge << 24 | in_main << 16 | in_main << 8 | in_edge;
	osd_shape_comm_draw_2color_frame_vertical_y(in_addr, in_x, in_y + 3, in_h - 6, in_gw, Y1Patten);
	osd_shape_comm_draw_2color_frame_vertical_y(in_addr, in_x + in_w - 4, in_y + 3, in_h - 6, in_gw, Y1Patten);
}

/**
 *	@brief Draw a donut's U and V
 *	@param in_addr: draw address
 *			in_x:position of X
 *			in_y:position of Y
 *			in_w:Width
 *			in_h:Height
 *			in_gw: global width
 *			in_main: body's color of U or V
 *	@return None
 *	@note		the U/V of outline must be as same as body
 *	@attention	None
 */
static VOID osd_shape_comm_draw_2color_frame_cc(ULONG in_addr, ULONG in_x, ULONG in_y, ULONG in_w, ULONG in_h, ULONG in_gw, USHORT in_main)
{
	UINT32 cPatten = (in_main<<16) | in_main;
	osd_shape_comm_draw_2color_frame_horizontal_cc(in_addr, in_x, in_y, in_w, in_gw, cPatten);
	osd_shape_comm_draw_2color_frame_horizontal_cc(in_addr, in_x, in_y+1, in_w, in_gw, cPatten);

	osd_shape_comm_draw_2color_frame_horizontal_cc(in_addr, in_x, in_y + in_h - 1, in_w, in_gw, cPatten);
	osd_shape_comm_draw_2color_frame_horizontal_cc(in_addr, in_x, in_y + in_h - 2, in_w, in_gw, cPatten);

	osd_shape_comm_draw_2color_frame_vertical_cc(in_addr, in_x, in_y, in_h, in_gw, cPatten);
	osd_shape_comm_draw_2color_frame_vertical_cc(in_addr, in_x + in_w - 4, in_y, in_h, in_gw, cPatten);
}

// Draw a circle by fix data
static FJ_ERR_CODE osd_shape_comm_draw_circle(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data,  T_POS circle_position, T_REPEAT_DRAW* circle_data, USHORT circle_data_size)
{
	FJ_ERR_CODE ret =FJ_ERR_OK;
	INT32 i,k;
	T_Rect rect_data;
	INT32 increament_index = 1;
	USHORT screen_size[2];
	T_Rect screen_rect_data;
	SHORT	StartX;

	BF_Osd_Attr_Get_Draw_Area_Size(disp_no_data, screen_size);
	screen_rect_data.Postion.StartX = 0;
	screen_rect_data.Postion.StartY = 0;
	screen_rect_data.Width  = screen_size[D_BF_OSD_ATTR_INDEX_WIDTH];
	screen_rect_data.Height  = screen_size[D_BF_OSD_ATTR_INDEX_LINES];
	//Convert to draw area's realational coordinate
	BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(disp_no_data,&circle_position);
	rect_data.Postion.StartY= circle_position.StartY;
	StartX = circle_position.StartX ;
	rect_data.Height = 1;

	ULONG	osd_layer_address = 0;
	INT32 index =0;
	for(k=0;k<2;k++){
		for(i = 0;i<circle_data_size; i++){
			rect_data.Postion.StartX = StartX + circle_data[index].draw_startX;
			rect_data.Width = circle_data[index].draw_width;
			rect_data.Height = 1;
			if(BF_Osd_Draw_Comm_Get_Overlap_Rect(screen_rect_data,rect_data,&rect_data) ){
				BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(&rect_data.Postion, &osd_layer_address, disp_no_data);
#if FJ_OSD_COMM_RGBAXXXX == 2
			// RGBA4444
				OSD_USHORT_INT color_data = (OSD_USHORT_INT)BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.body_color.word);
#else
				OSD_USHORT_INT color_data =(OSD_USHORT_INT)gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.body_color.word;					// RGBA8888
#endif
				BF_Osd_General_Fill_Box_For_GR(color_data, screen_rect_data.Width, (OSD_USHORT_INT*)osd_layer_address, rect_data.Width, rect_data.Height);
			}
			rect_data.Postion.StartY++;
			index +=increament_index;
		}
		index = circle_data_size - 1;
		increament_index =-1;//Second time loop: 
	}

	return ret;
}

/**
Draw a horizontal line
@param[in] bDisplayID : id is valid when Attr is as following:<br>
			@ref E_OSDLayer_LCD_OSD0<br>
			@ref E_OSDLayer_LCD_OSD1<br>
			@ref E_OSDLayer_HDMI_OSD0<br>
@param[in]	line_start : The start coordination of line
@param[in]	end_x : The end X coordination of line
 */
static FJ_ERR_CODE osd_shape_comm_draw_1line( const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, T_POS line_start, SHORT end_x)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	T_Rect get_draw_part;

	if(line_start.StartX> end_x){
		//Exchange X 
		get_draw_part.Postion.StartX = end_x;
		get_draw_part.Width =  line_start.StartX - end_x +1;
	}
	else{
		get_draw_part.Postion.StartX = line_start.StartX;
		get_draw_part.Width = end_x - line_start.StartX +1;
	}
	get_draw_part.Postion.StartY = line_start.StartY;
	get_draw_part.Height =  1;//dumy

	ULONG	osd_layer_address = 0;
	OSD_USHORT_INT* write_address;
	SHORT i;
	USHORT screen_size[2];
	T_Rect screen_rect_data;

	BF_Osd_Attr_Get_Draw_Area_Size(disp_no_data, screen_size);
	screen_rect_data.Postion.StartX = 0;
	screen_rect_data.Postion.StartY = 0;
	screen_rect_data.Width  = screen_size[D_BF_OSD_ATTR_INDEX_WIDTH];
	screen_rect_data.Height  = screen_size[D_BF_OSD_ATTR_INDEX_LINES];

	if(BF_Osd_Draw_Comm_Get_Overlap_Rect(screen_rect_data,get_draw_part,&get_draw_part) ){

		BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(&get_draw_part.Postion, &osd_layer_address, disp_no_data);
		// Make its to VGR buffer
		write_address = (OSD_USHORT_INT*)osd_layer_address;
#if FJ_OSD_COMM_RGBAXXXX == 2
			// RGBA4444
		OSD_USHORT_INT color_data = (OSD_USHORT_INT)BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.body_color.word);
#else
		OSD_USHORT_INT color_data =(OSD_USHORT_INT)gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.body_color.word;					// RGBA8888
#endif
		for(i = get_draw_part.Width; i>0; i--){
			*write_address =color_data;
			write_address++;
		}
	}
	return ret;

}
/**
Draw a triganle from 2 lines
@param[in] bDisplayID : id is valid when Attr is as following:<br>
			@ref E_OSDLayer_LCD_OSD0<br>
			@ref E_OSDLayer_LCD_OSD1<br>
			@ref E_OSDLayer_HDMI_OSD0<br>
@param[in]	start : The point that is triangle's right angle
		//-------------(end_triangle_y)
		// -         -
		//	 -		-
		//	   -  -
		//		 - (start)
@param[in]	end_triangle_y : the Y of end point
@param[in]	k_left1 : the K of left triangle's line
 */
static FJ_ERR_CODE osd_shape_comm_draw_triangle_from_2lines( const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, const T_POS* start, SHORT end_triangle_y, float k_left)
{
	SHORT increatment ;
	float k_right = -1/k_left;
	T_POS line_start = {0, 0}; 
	FJ_ERR_CODE ret =FJ_ERR_OK;

	if(start->StartY <end_triangle_y){
		//-------------
		// -         -
		//	 -		-
		//	   -  -
		//		 -
		increatment =1;
        end_triangle_y++;//Include end_triangle_y
	}
	else{
		//		 -
		//	   -  -
		//	 -		-
		// -         -
		//-------------
		increatment = -1;
        end_triangle_y--;//Include end_triangle_y
	}
	for(line_start.StartY = start->StartY; line_start.StartY != end_triangle_y; line_start.StartY += increatment) {
		// Line 3's equation
		//Y-y3=k_left(X-x3) -> k_left = -1/k_right
		//X= (Y-y3)/k_left+x3 -> = -k_right*(Y-y3)+x3  
		line_start.StartX = rint(start->StartX - k_right * (line_start.StartY - start->StartY));
		// Line 4's equation
		//Y-y3=k_right(X-x3) -> k_right = -1/k_left
		//X= (Y-y3)/k_right+x3 -> = -k_left*(Y-y3)+x3  
		SHORT down_triangle_end = rint(start->StartX - k_left * (line_start.StartY - start->StartY));
		ret |=osd_shape_comm_draw_1line(disp_no_data, line_start, down_triangle_end);
	}
	return ret;
}
/**
Draw a triganle from 2 lines
@param[in] bDisplayID : id is valid when Attr is as following:<br>
			@ref E_OSDLayer_LCD_OSD0<br>
			@ref E_OSDLayer_LCD_OSD1<br>
			@ref E_OSDLayer_HDMI_OSD0<br>
@param[in]	start : The point that is triangle's right angle
@param[in]	end_triangle_y : the Y of end point
@param[in]	k : the K of lines that vertical each other
 */
static FJ_ERR_CODE osd_shape_com_draw_parallelogram( const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, const T_POS* start, const T_POS* end, float k)
{
	//          *              *(x4,x4)
	//		   *              *
	//        *              *
	//       *(x2,y2)       *
	k = 1/k;
	FJ_ERR_CODE ret =FJ_ERR_OK;
	T_POS line_start = {0, 0};
	for(line_start.StartY = start->StartY; line_start.StartY<=end->StartY; line_start.StartY++) {
		// Line 1's equation
		//Y-y2=k(X-x2)
		//X= (Y-y2)/k+x2 -> = (Y-y2)/k + x2  
		line_start.StartX = rint(start->StartX + k * (line_start.StartY - start->StartY));
		// Line 4's equation
		//Y-y4=k(X-x4) 
		//X= (Y-y4)/k+x4 -> = (Y-y4)/k+x4  
		SHORT parallelogram_end = rint(end->StartX + k * (line_start.StartY - end->StartY));
		ret |=osd_shape_comm_draw_1line(disp_no_data, line_start, parallelogram_end);
	}
	return ret;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
///////////////////////////////////////////////////
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_COMM_CODE"
FJ_ERR_CODE BF_Osd_Shape_Comm_Check_Coordinates( USHORT width, USHORT heigh)
{
	FJ_ERR_CODE ret = FJ_ERR_NG_INPUT_PARAM;

	if ((width > 0) && (heigh > 0)) {
		ret = FJ_ERR_OK;
	}
	return ret;
}

FJ_ERR_CODE	BF_Osd_Shape_Comm_Draw_Shape_GR(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data,
						E_FJ_OSD_SHAPE_TYPE wType, T_Rect obj_wnd, T_Rect dispWnd,
						const T_IconColor Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_END],
						T_POS const* const frame_theckness, E_DrawMode DrawMode, BYTE alpha )
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	T_BF_OSD_SHAPE_COMM_SHAPE_INFO rect_data;
	T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_END];
	USHORT screen_size[2];//[0]: width [1]:height

	BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(disp_no_data, &obj_wnd.Postion);

	dispWnd.Postion.StartX += (obj_wnd.Postion.StartX);
	dispWnd.Postion.StartY += (obj_wnd.Postion.StartY);

	//Get campus size
	BF_Osd_Attr_Get_Draw_Area_Size(disp_no_data,screen_size);
	if (BF_Osd_Draw_Comm_Is_Rect_At_LCD_Inside(screen_size, &dispWnd)) {
		rect_data.Rect = obj_wnd;

		switch (wType) {
			case E_FJ_OSD_SHAPE_TYPE_RECT:
				display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT].rect_data.Rect      = rect_data.Rect;
				display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT].Color_Blending.word = Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY].word;

				if(DrawMode == E_DrawMode_Speed_Optimized) {
					//Speed draw
					osd_shape_comm_5_parts_rect_gr(disp_no_data, screen_size, &dispWnd, &display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT], 1);
				}
				else{
					//Effect draw
					osd_shape_comm_5_parts_rect_gr_blending(disp_no_data, screen_size, &dispWnd, &display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT], 1, alpha);
				}
				break;

			case E_FJ_OSD_SHAPE_TYPE_RECT_FRAME:
				// Divide rectangle to 5 rectangles
				BF_Osd_Shape_Comm_Divide_Rect_5_Parts(&rect_data, frame_theckness, display_data);
				// Set color
				display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT].Color_Blending.word   =
				display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT].Color_Blending.word  =
				display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP].Color_Blending.word    =
				display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM].Color_Blending.word =	Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word;

				// Draw the 5 rectangles
				if(DrawMode == E_DrawMode_Speed_Optimized) {
					//Speed draw
					osd_shape_comm_5_parts_rect_gr(disp_no_data, screen_size, &dispWnd,
												&display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT],
												E_BF_OSD_SHAPE_COMM_DIV_RECT_END - E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT);
				}
				else{
					//Effect draw
					osd_shape_comm_5_parts_rect_gr_blending(disp_no_data, screen_size, &dispWnd,
												&display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT],
												E_BF_OSD_SHAPE_COMM_DIV_RECT_END - E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT, alpha);
				}
				break;

			case E_FJ_OSD_SHAPE_TYPE_FILLED_RECT_FRAME:
				// Divide rectangle to 5 rectangles
				BF_Osd_Shape_Comm_Divide_Rect_5_Parts(&rect_data, frame_theckness, display_data);
				// Set color
				display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT].Color_Blending.word   =
				display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT].Color_Blending.word  =
				display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP].Color_Blending.word    =
				display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM].Color_Blending.word = Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word;
				display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_CENTER].Color_Blending.word = Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY].word;

				// Divide rectangle to 5 rectangles
				if(DrawMode == E_DrawMode_Speed_Optimized) {
					//Speed draw
					osd_shape_comm_5_parts_rect_gr(disp_no_data, screen_size, &dispWnd, display_data, E_BF_OSD_SHAPE_COMM_DIV_RECT_END);
				}
				else{
					//Effect draw
					osd_shape_comm_5_parts_rect_gr_blending(disp_no_data, screen_size, &dispWnd, display_data, E_BF_OSD_SHAPE_COMM_DIV_RECT_END, alpha);
				}
				break;

			case E_FJ_OSD_SHAPE_TYPE_DONUT_RECT_FRAME:
				if ((frame_theckness->StartX >= 4) && (frame_theckness->StartY >= 4)) {
					ret = osd_shape_comm_draw_donut(disp_no_data, screen_size, &rect_data, &dispWnd, Color_Blending, frame_theckness, DrawMode, alpha);
				}
				else {
					ret = FJ_ERR_NG_INPUT_PARAM;
				}
				break;

			case E_FJ_OSD_SHAPE_TYPE_CIRCLE:		// FALL THROUGH
			case E_FJ_OSD_SHAPE_TYPE_FILLED_CIRCLE:	// FALL THROUGH
			case E_FJ_OSD_SHAPE_TYPE_ROUND_FRAME:	// FALL THROUGH
			case E_FJ_OSD_SHAPE_TYPE_FILLED_ROUND_FRAME:
			{
				if (((obj_wnd.Height & 1) == 0) && ((obj_wnd.Width & 1) == 0)) {
					T_BF_OSD_ICON_COMM_DRAW_ICON_INFO work_shape_data;

					rect_data.wType = wType;
					work_shape_data.string_iocn_pos = obj_wnd.Postion; // Set work shape to obj

					if (BF_Osd_Draw_Comm_Is_Rect_At_LCD_Inside(screen_size, &obj_wnd) &&
						BF_Osd_Draw_Comm_Get_Overlap_Rect(obj_wnd, dispWnd, &work_shape_data.display_window)) {

						BOOL	is_patial_draw;
						ULONG	draw_buffer_top_address;
						ULONG	osd_layer_address = 0;
						T_POS	frame_size;
						T_BF_OSD_DRAW_COMM_DRAW_SHAPE_INFO Write_infor;

						frame_size.StartY =
						frame_size.StartX = gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.outline_size.StartX;

						Write_infor.drwa_layer = *disp_no_data;
						Write_infor.write_data.global_size = 
						Write_infor.write_data.line_feed = screen_size[D_BF_OSD_ATTR_INDEX_WIDTH];

						is_patial_draw = osd_shape_comm_set_draw_buffer_addr(&dispWnd, &rect_data, &Write_infor, &draw_buffer_top_address);
						if (is_patial_draw) {
							// Copy display buffer content to work buffer
							// Get VGR write information
							BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(&work_shape_data.display_window.Postion, &osd_layer_address, disp_no_data);

							memset((BYTE*)draw_buffer_top_address, 0, screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] * screen_size[D_BF_OSD_ATTR_INDEX_LINES] * FJ_OSD_COMM_RGBAXXXX);

							// because of work_buffer[D_BF_OSD_GENERAL_YCC_Y] may be used at round rectangle draw
							work_shape_data.icon_data.mBuffer = (CHAR*)draw_buffer_top_address;
							work_shape_data.icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] = screen_size[D_BF_OSD_ATTR_INDEX_WIDTH];
							work_shape_data.icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES] = screen_size[D_BF_OSD_ATTR_INDEX_LINES];
							//
							BF_Osd_Icon_Comm_Recalculate_Icon_Addr(E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON, &work_shape_data);
							BF_Osd_General_Copy_Box_To_Box(osd_layer_address,
														   screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] * FJ_OSD_COMM_RGBAXXXX,
														   screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] * FJ_OSD_COMM_RGBAXXXX,
														   (ULONG)work_shape_data.icon_data.mBuffer,
														   work_shape_data.display_window.Width * FJ_OSD_COMM_RGBAXXXX,
														   work_shape_data.display_window.Height);
						}
						ret = osd_shape_comm_draw_circle_round_rect(disp_no_data->disp_block_no, draw_buffer_top_address, &rect_data, &frame_size, &Write_infor);
						if (is_patial_draw && ret == FJ_ERR_OK) {
							if(DrawMode == E_DrawMode_Speed_Optimized) {
								//Speed draw
								BF_Osd_General_Copy_Box_To_Box((ULONG)work_shape_data.icon_data.mBuffer,	/* pgr0872 */
														   screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] * FJ_OSD_COMM_RGBAXXXX,
														   screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] * FJ_OSD_COMM_RGBAXXXX,
														   osd_layer_address,
														   work_shape_data.display_window.Width * FJ_OSD_COMM_RGBAXXXX,
														   work_shape_data.display_window.Height);
							}
							else{
								//Effect draw
								BF_Osd_Draw_Comm_Blend_Box_To_Box((OSD_USHORT_INT*)work_shape_data.icon_data.mBuffer,	/* pgr0872 */
														   screen_size[D_BF_OSD_ATTR_INDEX_WIDTH],
														   screen_size[D_BF_OSD_ATTR_INDEX_WIDTH],
														   (OSD_USHORT_INT*)osd_layer_address,
														   work_shape_data.display_window.Width,
														   work_shape_data.display_window.Height, alpha);
							}
						}
					}
				}
				else {
					ret = FJ_ERR_NG_INPUT_PARAM;
				}
			}
			break;

			default:
				ret = FJ_ERR_NG_INPUT_PARAM;
		}
	}
	return ret;
}

VOID BF_Osd_Shape_Comm_Draw_Rect_GR(T_Rect rect_data, USHORT grihsize, T_IconColor color_yuv, ULONG osd_layer_address)
{
	BF_Osd_General_Fill_Box_For_GR((OSD_USHORT_INT)color_yuv.word, grihsize, (OSD_USHORT_INT*)osd_layer_address, rect_data.Width, rect_data.Height);
}

FJ_ERR_CODE BF_Osd_Shape_Comm_Draw_Triangle(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, USHORT triangle_x, USHORT triangle_y,
											USHORT width, USHORT heigh, E_FJ_OSD_SHAPE_TYPE type)
{
	FJ_ERR_CODE ret;
	SHORT	i;
	SHORT	line_x1, line_x2, line_y1, line_y2, slant_x, slant_y;
	SHORT	write_data_num;
	int		loop_start, loop_increat;
	USHORT	grihsize[2];//display size [0]: width [1]:height
	T_POS	point_at_diagonal_line;
	T_POS	point_at_straight_line;
	T_POS*	draw_start_point;
	T_POS*	draw_end_point;
	T_POS	triangle_xy = {triangle_x, triangle_y};

	ret = BF_Osd_Shape_Comm_Check_Coordinates(width, heigh);
	loop_start   = 0;
	loop_increat = 1;

	if (ret == FJ_ERR_OK) {
		ret = BF_Osd_Draw_Comm_Is_Rect_Draw_Area_Inside(disp_no_data, triangle_xy, width, heigh);
	}

	BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(disp_no_data, &triangle_xy);

	// Calculate coordinates
	triangle_xy.StartX += gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].common.left_top_pos.StartX;
	triangle_xy.StartY += gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].common.left_top_pos.StartY;

	if (ret == FJ_ERR_OK) {
		// Get diagonal line falmulate

		point_at_straight_line.StartY = triangle_y;
		switch (type) {
			case E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_LT:
				// (x,y)
				// +------------+(x1,y1)
				// |		+
				// |	+
				// +(x2,y2)
				line_x1 = triangle_xy.StartX + width;
				line_y1 = triangle_xy.StartY;
				line_x2 = triangle_xy.StartX;
				line_y2 = triangle_xy.StartY + heigh;
				point_at_straight_line.StartX = triangle_xy.StartX;
				draw_start_point = &point_at_straight_line;
				draw_end_point = &point_at_diagonal_line;
				break;

			case E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_RT:
				// (x,y)=(x1,y1)
				// +------------+
				//	  +			|
				//		  +		|
				//				+(x2,y2)
				line_x1 = triangle_xy.StartX;
				line_y1 = triangle_xy.StartY;
				line_x2 = triangle_xy.StartX + width;
				line_y2 = triangle_xy.StartY + heigh;
				point_at_straight_line.StartX = triangle_xy.StartX + width;
				draw_end_point = &point_at_straight_line;
				draw_start_point = &point_at_diagonal_line;
				break;

			case E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_LB:
				// (x,y)=(x2,y2)
				// +
				// |	+
				// |		+
				// +------------+(x1,y1)
				line_x2 = triangle_xy.StartX;
				line_y2 = triangle_xy.StartY;
				line_x1 = triangle_xy.StartX + width;
				line_y1 = triangle_xy.StartY + heigh;
				point_at_straight_line.StartX = triangle_xy.StartX;
				draw_start_point = &point_at_straight_line;
				draw_end_point = &point_at_diagonal_line;
				loop_increat = -1;
				break;

			case E_FJ_OSD_SHAPE_TYPE_TRIANGLE_DIR_RB:
				// (x,y)
				//				+(x2,y2)
				//			+	|
				//		+		|
				// +------------+
				//(x1,y1)
				line_x2 = triangle_xy.StartX + width;
				line_y2 = triangle_xy.StartY;
				line_x1 = triangle_xy.StartX;
				line_y1 = triangle_xy.StartY + heigh;
				point_at_straight_line.StartX = triangle_xy.StartX + width;
				draw_end_point = &point_at_straight_line;
				draw_start_point = &point_at_diagonal_line;
				loop_increat = -1;
				break;

			default:
				// Parameter error.
				return FJ_ERR_NG_INPUT_PARAM;
		}
		slant_x = (line_x1 - line_x2);
		slant_y = (line_y1 - line_y2);

		if (slant_y > 0) {
			// When LB/RB, the bottom line's Y should be -1
			line_y1--;
		}

		// Draw triangle to VGRch.
		ULONG address = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(Sdram_Map_Osd_Get_OSD_Chg_Addr(disp_no_data));
		T_IconColor body_colr;
		BF_Osd_Attr_Get_Draw_Area_Size(disp_no_data,grihsize);

		// Pointer initialize
#if FJ_OSD_COMM_RGBAXXXX == 2
		// RGBA4444
			body_colr.word = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.body_color.word);
#else
			body_colr.word = (gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.body_color.word);
		// RGBA8888
#endif
		if (body_colr.RGBA.A) {		/* pgr0872 */
			int j;
			OSD_USHORT_INT* write_address;

			for (i = 0; i < heigh; i++) {
				point_at_straight_line.StartY =
				point_at_diagonal_line.StartY = line_y1 + loop_start;
				point_at_diagonal_line.StartX = (short)((float)(slant_x * (point_at_diagonal_line.StartY - line_y1)) / (float)slant_y + (float)line_x1);

				write_data_num = draw_end_point->StartX - draw_start_point->StartX;

				// Set Y
				write_address = (OSD_USHORT_INT*)address;
				write_address += (draw_start_point->StartX + draw_start_point->StartY * grihsize[D_BF_OSD_ATTR_INDEX_WIDTH]);

				for (j = 0; j < write_data_num; j++) {
					*write_address = (OSD_USHORT_INT)body_colr.word;
					write_address++;
				}
				loop_start += loop_increat;
			}
		}
	}
	return ret;
}

VOID BF_Osd_Shape_Comm_Divide_Rect_5_Parts(T_BF_OSD_SHAPE_COMM_SHAPE_INFO const* const rect_data, T_POS const* const Outline_Size,
										   T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_END])
{
	// divide the roud to following
	//+-------------------------------------+
	//|	                2                   |
	//+-- +-----------------------------+---+
	//|	0 |								| 1 |
	//|	  |				4				|   |
	//+---+-----------------------------+---+
	//|	                3				    |
	//+-------------------------------------+

	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT].rect_data   =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT].rect_data  =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP].rect_data    =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM].rect_data =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_CENTER].rect_data = *rect_data;


	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT].rect_data.Rect.Width  =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT].rect_data.Rect.Width = Outline_Size->StartX;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT].rect_data.Rect.Postion.StartX = rect_data->Rect.Postion.StartX + (rect_data->Rect.Width - Outline_Size->StartX);

	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_CENTER].rect_data.Rect.Width = rect_data->Rect.Width - (Outline_Size->StartX << 1);

	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_TOP].rect_data.Rect.Height    =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM].rect_data.Rect.Height = Outline_Size->StartY;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_CENTER].rect_data.Rect.Postion.StartX = rect_data->Rect.Postion.StartX + Outline_Size->StartX;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_BOTTOM].rect_data.Rect.Postion.StartY = rect_data->Rect.Postion.StartY + (rect_data->Rect.Height - Outline_Size->StartY);

	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT].rect_data.Rect.Postion.StartY   =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT].rect_data.Rect.Postion.StartY  =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_CENTER].rect_data.Rect.Postion.StartY = rect_data->Rect.Postion.StartY + Outline_Size->StartY;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_LEFT].rect_data.Rect.Height   =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_RIGHT].rect_data.Rect.Height  =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_RECT_CENTER].rect_data.Rect.Height = rect_data->Rect.Height - (Outline_Size->StartY << 1);
}

FJ_ERR_CODE BF_Osd_Shape_Comm_Divide_Round_11_Parts(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no,
													T_BF_OSD_SHAPE_COMM_SHAPE_INFO const* const round_rect,
													T_BF_OSD_SHAPE_COMM_SHAPE_INFO round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_END],
													T_POS const* const frame_size)
{
	FJ_ERR_CODE ret = FJ_ERR_NG_INPUT_PARAM;
	USHORT rect_width_corner = (gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.round_rect_radius << 1);

	if (BF_Osd_General_Get_Min(round_rect->Rect.Height, round_rect->Rect.Width) > rect_width_corner) {
		// can be divisible by 11

		// divide the roud to following
		//+-+---+-----------------------------+-----+
		//|     |              4              |     |
		//+     +-----------------------------+     +
		//|	0   |	        8                 | 1   |
		//+-+---+-----------------------------+---+-+
		//|	|                                     | |
		//|6|                   9			      |7|
		//+-----+-----------------------------+---+-+
		//|	2   |			10                | 3   |
		//+     +-----------------------------+     |
		//+     |              5              |     |
		//+-----+-----------------------------+-----+

		if (gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.round_rect_radius > frame_size->StartX) {
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_TOP].Rect.Postion.StartX    =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_BOTTOM].Rect.Postion.StartX =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_LEFT_FRAME].Rect.Postion.StartX    = round_rect->Rect.Postion.StartX;

			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_TOP].Rect.Postion.StartY  =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_TOP].Rect.Postion.StartY =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_TOP_FRAME].Rect.Postion.StartY   = round_rect->Rect.Postion.StartY;

			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_TOP_FRAME].Rect.Postion.StartX    =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_BOTTOM_BODY].Rect.Postion.StartX  =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_TOP_BODY].Rect.Postion.StartX     =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_BOTTOM_FRAME].Rect.Postion.StartX =	round_rect->Rect.Postion.StartX + (rect_width_corner >> 1);

			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_CENCER].Rect.Postion.StartX = round_rect->Rect.Postion.StartX + (frame_size->StartX);
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_CENCER].Rect.Width = round_rect->Rect.Width - (frame_size->StartX << 1);

			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_TOP_FRAME].Rect.Width    =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_BOTTOM_FRAME].Rect.Width = round_rect->Rect.Width - (rect_width_corner);

			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_TOP_FRAME].Rect.Height    =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_BOTTOM_FRAME].Rect.Height = frame_size->StartY;

			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_TOP].Rect.Postion.StartX    =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_BOTTOM].Rect.Postion.StartX = round_rect->Rect.Postion.StartX + (round_rect->Rect.Width - rect_width_corner);

			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_BOTTOM].Rect.Postion.StartY =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_BOTTOM].Rect.Postion.StartY  = round_rect->Rect.Postion.StartY + (round_rect->Rect.Height - rect_width_corner);

			//4 corners circle width and highe
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_TOP].Rect.Height     =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_BOTTOM].Rect.Height  =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_TOP].Rect.Height    =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_BOTTOM].Rect.Height =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_TOP].Rect.Width     =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_BOTTOM].Rect.Width  =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_TOP].Rect.Width    =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_BOTTOM].Rect.Width =	rect_width_corner;

			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_LEFT_FRAME].Rect.Postion.StartY  =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_RIGHT_FRAME].Rect.Postion.StartY = round_rect->Rect.Postion.StartY + (rect_width_corner >> 1);

			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_LEFT_FRAME].Rect.Width  =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_RIGHT_FRAME].Rect.Width = frame_size->StartX;

			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_LEFT_FRAME].Rect.Height  =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_RIGHT_FRAME].Rect.Height = round_rect->Rect.Height - (rect_width_corner);

			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_BOTTOM_BODY].Rect.Height =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_TOP_BODY].Rect.Height    = rect_width_corner - frame_size->StartY;

			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_BOTTOM_BODY].Rect.Width =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_TOP_BODY].Rect.Width    = round_rect->Rect.Width - (rect_width_corner >> 1);

			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_RIGHT_FRAME].Rect.Postion.StartX = round_rect->Rect.Postion.StartX + (round_rect->Rect.Width - frame_size->StartX);
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_BOTTOM_FRAME].Rect.Postion.StartY = round_rect->Rect.Postion.StartY + (round_rect->Rect.Height - frame_size->StartY);
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_CENCER].Rect.Postion.StartY = round_rect->Rect.Postion.StartY + (rect_width_corner >> 1);
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_CENCER].Rect.Height = round_rect->Rect.Height - (rect_width_corner);
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_TOP_BODY].Rect.Postion.StartY = round_rect->Rect.Postion.StartY + frame_size->StartY;
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_BOTTOM_BODY].Rect.Postion.StartY = round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_CENCER].Rect.Postion.StartY + round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_CENCER].Rect.Height;

			if (round_rect->wType == E_FJ_OSD_SHAPE_TYPE_ROUND_FRAME) {
				round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_TOP].wType     =
				round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_TOP].wType    =
				round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_BOTTOM].wType =
				round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_BOTTOM].wType  = E_FJ_OSD_SHAPE_TYPE_CIRCLE;
			}
			else {
				round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_TOP].wType     =
				round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_TOP].wType    =
				round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_RIGHT_BOTTOM].wType =
				round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_CIRCLE_LEFT_BOTTOM].wType  = E_FJ_OSD_SHAPE_TYPE_FILLED_CIRCLE;
			}
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_TOP_FRAME].wType    =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_LEFT_FRAME].wType   =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_BOTTOM_FRAME].wType =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_CENCER].wType       =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_TOP_BODY].wType     =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_BOTTOM_BODY].wType  =
			round_data_11_parts[E_BF_OSD_SHAPE_COMM_ROUND_POS_RECT_RIGHT_FRAME].wType  = E_FJ_OSD_SHAPE_TYPE_RECT_FRAME;
			ret = FJ_ERR_OK;
		}
	}
	return ret;
}

VOID BF_Osd_Shape_Comm_Divide_Donut_13_Parts(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no,
											T_BF_OSD_SHAPE_COMM_SHAPE_INFO const* const rect_data, T_POS const* const Outline_Size,
											T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_THE_END])
{
	// divide the to following
	//  theck
	//|<---->|
	//+-------------------------------------+------
	//|	                exterior top	    |   Theck
	//+-+---------------------------------+-+
	//|e|       Center top                |e|
	//|x|                                 |x|
	//|t+--+---------------------------+--+t+
	//|e|C |    interior top		   |C |e|
	//|r|  +-+-----------------------+-+  |r|------
	//|	|L |i|			Center       |i|R |	|
	//|L|  |n|			             |n|  |R|
	//| |  |t|			             |t|  | |
	//| |  |e|			             |e|  | |
	//| |  |r|			             |r|  | |
	//|	|  |L|			             |R|  |	|
	//| |  +-+-----------------------+-+  | |
	//| |  |    interior B             |  | |
	//| +--+---------------------------+--+ |
	//| |       Center B                  | |
	//| |                                 | |
	//+-+---------------------------------+-+
	//|	                exterior B          |
	//+-------------------------------------+

	//exterior and interior Fram size must be 1
	USHORT exterior_and_interior_Fram_size = gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].shape.Donut_Rect_frame_size;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_TOP].rect_data.Rect.Height    =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_BOTTOM].rect_data.Rect.Height =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_LEFT].rect_data.Rect.Width   =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_RIGHT].rect_data.Rect.Width  =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_TOP].rect_data.Rect.Height    =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_BOTTOM].rect_data.Rect.Height =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_LEFT].rect_data.Rect.Width   =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_RIGHT].rect_data.Rect.Width  = exterior_and_interior_Fram_size;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_TOP].rect_data.Rect.Width = display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_BOTTOM].rect_data.Rect.Width = rect_data->Rect.Width;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_BOTTOM].rect_data.Rect.Postion.StartY = rect_data->Rect.Postion.StartY + (rect_data->Rect.Height - exterior_and_interior_Fram_size); // 3 is EXTERIOR top and bottom 's Height;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_TOP].rect_data.Rect.Postion.StartX    =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_BOTTOM].rect_data.Rect.Postion.StartX =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_LEFT].rect_data.Rect.Postion.StartX   = rect_data->Rect.Postion.StartX;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_TOP].rect_data.Rect.Postion.StartY = rect_data->Rect.Postion.StartY;
	// End of EXTERIOR_TOP and BOTTOM

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_TOP].rect_data.Rect.Width    =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_BOTTOM].rect_data.Rect.Width =	rect_data->Rect.Width - ((Outline_Size->StartX - exterior_and_interior_Fram_size) << 1); //3 is INTERIOR left and right 's width;


	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_TOP].rect_data.Rect.Postion.StartX    =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_BOTTOM].rect_data.Rect.Postion.StartX = rect_data->Rect.Postion.StartX + Outline_Size->StartX - exterior_and_interior_Fram_size;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_TOP].rect_data.Rect.Postion.StartY = rect_data->Rect.Postion.StartY + (Outline_Size->StartY - exterior_and_interior_Fram_size);

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_BOTTOM].rect_data.Rect.Postion.StartY = rect_data->Rect.Postion.StartY + (rect_data->Rect.Height - Outline_Size->StartY);
	// End of INTERIOR_TOP and BOTTOM

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_RIGHT].rect_data.Rect.Postion.StartX
		= rect_data->Rect.Postion.StartX + rect_data->Rect.Width - exterior_and_interior_Fram_size;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_LEFT].rect_data.Rect.Postion.StartY  =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_RIGHT].rect_data.Rect.Postion.StartY =	rect_data->Rect.Postion.StartY + exterior_and_interior_Fram_size;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_LEFT].rect_data.Rect.Height  =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_EXTERIOR_RIGHT].rect_data.Rect.Height = rect_data->Rect.Height - exterior_and_interior_Fram_size*2;
	// End of EXTERIOR left and right

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_LEFT].rect_data.Rect.Height  =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_RIGHT].rect_data.Rect.Height = rect_data->Rect.Height - (Outline_Size->StartY << 1);

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_LEFT].rect_data.Rect.Postion.StartY  =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_RIGHT].rect_data.Rect.Postion.StartY =	rect_data->Rect.Postion.StartY + Outline_Size->StartY;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_LEFT].rect_data.Rect.Postion.StartX = rect_data->Rect.Postion.StartX + Outline_Size->StartX - exterior_and_interior_Fram_size;
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_RIGHT].rect_data.Rect.Postion.StartX = rect_data->Rect.Postion.StartX + rect_data->Rect.Width - Outline_Size->StartX;
	// End of INTERIOR left and right

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_TOP].rect_data.Rect.Height    =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_BOTTOM].rect_data.Rect.Height = Outline_Size->StartY - 2*exterior_and_interior_Fram_size;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_TOP].rect_data.Rect.Width    =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_BOTTOM].rect_data.Rect.Width = rect_data->Rect.Width - 2*exterior_and_interior_Fram_size;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_TOP].rect_data.Rect.Postion.StartX    =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_BOTTOM].rect_data.Rect.Postion.StartX = rect_data->Rect.Postion.StartX + exterior_and_interior_Fram_size;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_TOP].rect_data.Rect.Postion.StartY = rect_data->Rect.Postion.StartY + exterior_and_interior_Fram_size;
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_BOTTOM].rect_data.Rect.Postion.StartY = display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_BOTTOM].rect_data.Rect.Postion.StartY + exterior_and_interior_Fram_size;
	// End of CENTER TOP and BOTTOM

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_LEFT].rect_data.Rect.Width  =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_RIGHT].rect_data.Rect.Width = Outline_Size->StartX - 2*exterior_and_interior_Fram_size;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_LEFT].rect_data.Rect.Height  =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_RIGHT].rect_data.Rect.Height = display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_LEFT].rect_data.Rect.Height + exterior_and_interior_Fram_size*2;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_LEFT].rect_data.Rect.Postion.StartY  =
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_RIGHT].rect_data.Rect.Postion.StartY = display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_TOP].rect_data.Rect.Postion.StartY;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_LEFT].rect_data.Rect.Postion.StartX = rect_data->Rect.Postion.StartX + exterior_and_interior_Fram_size;
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER_RIGHT].rect_data.Rect.Postion.StartX = display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_RIGHT].rect_data.Rect.Postion.StartX + exterior_and_interior_Fram_size;

	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER].rect_data.Rect.Postion.StartX = display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_TOP].rect_data.Rect.Postion.StartX + exterior_and_interior_Fram_size;
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER].rect_data.Rect.Postion.StartY = display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_TOP].rect_data.Rect.Postion.StartY + exterior_and_interior_Fram_size;
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER].rect_data.Rect.Width = display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_TOP].rect_data.Rect.Width - 2*exterior_and_interior_Fram_size;
	display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_CENTER].rect_data.Rect.Height = display_data[E_BF_OSD_SHAPE_COMM_DIV_DONUT_RECT_INTERIOR_LEFT].rect_data.Rect.Height;
}

///////////////////////////////////////////////////
//  Following are BASEFW_VIEW_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_VIEW_CODE"
VOID BF_Osd_Shape_Comm_Draw_2color_Frame(const ULONG in_addr[2], ULONG in_x, ULONG in_y, ULONG in_w, ULONG in_h, ULONG in_gw,
										 BYTE in_edge, BYTE in_main, BYTE in_cc1, BYTE in_cc2)
{
	osd_shape_comm_draw_2color_frame_y(in_addr[0],  in_x,  in_y,  in_w,  in_h,  in_gw, in_edge, in_main);

	in_y >>= 1;
	in_h >>= 1;
	USHORT cbcr=(in_cc1<<8) | in_cc2;
	osd_shape_comm_draw_2color_frame_cc(in_addr[1],  in_x,  in_y,  in_w,  in_h,  in_gw, cbcr);
}

FJ_ERR_CODE BF_Osd_Shape_Comm_Draw_Circle(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, T_Rect obj_wnd)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	switch(obj_wnd.Width){
		case D_OSD_SHAPE_COMM_CIRCLE_SIZE_1:
			ret = osd_shape_comm_draw_circle(disp_no_data, obj_wnd.Postion, gOsd_Shape_Comm_Circle_Size_02, D_OSD_SHAPE_COMM_CIRCLE_SIZE_1/2);
			break;
		case D_OSD_SHAPE_COMM_CIRCLE_SIZE_2:
			ret = osd_shape_comm_draw_circle(disp_no_data, obj_wnd.Postion, gOsd_Shape_Comm_Circle_Size_10, D_OSD_SHAPE_COMM_CIRCLE_SIZE_2/2);
			break;
		case D_OSD_SHAPE_COMM_CIRCLE_SIZE_3:
			ret = osd_shape_comm_draw_circle(disp_no_data, obj_wnd.Postion, gOsd_Shape_Comm_Circle_Size_24, D_OSD_SHAPE_COMM_CIRCLE_SIZE_3/2);
			break;
		case D_OSD_SHAPE_COMM_CIRCLE_SIZE_4:
			ret = osd_shape_comm_draw_circle(disp_no_data, obj_wnd.Postion, gOsd_Shape_Comm_Circle_Size_34, D_OSD_SHAPE_COMM_CIRCLE_SIZE_4/2);
			break;
		case D_OSD_SHAPE_COMM_CIRCLE_SIZE_5:
			ret = osd_shape_comm_draw_circle(disp_no_data, obj_wnd.Postion, gOsd_Shape_Comm_Circle_Size_42, D_OSD_SHAPE_COMM_CIRCLE_SIZE_5/2);
			break;
		default:{
			T_POS backup_outline_size =gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.outline_size;
			UINT32 backup_outline_color =gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.outline_color.word;

			gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.outline_size.StartX =
			gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.outline_size.StartY = 1;
			gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.outline_color.word = gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.body_color.word;
//			ret = FJ_OSDDrawShape(disp_no_data->layer_name, E_FJ_OSD_SHAPE_TYPE_FILLED_CIRCLE, obj_wnd.Postion.StartX, obj_wnd.Postion.StartY, obj_wnd.Width, obj_wnd.Height,disp_no_data->draw_area);
			//recovery
			gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.outline_color.word = backup_outline_color;
			gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.outline_size = backup_outline_size;
			break;
		}
	}
	return ret;
}

FJ_ERR_CODE BF_Osd_Shape_Comm_drawstraight(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, const FJ_LINE_COORDINATES* line, USHORT Thickness)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	USHORT screen_size[2];
	T_Rect screen_rect_data;

	T_Rect rect_data;
	if( (line->start_pos.StartX == line->end_pos.StartX) && ( line->start_pos.StartY == line->end_pos.StartY)) {
			//Point
			rect_data.Postion.StartX = line->start_pos.StartX;
			rect_data.Postion.StartY = line->start_pos.StartY - Thickness/2;
			rect_data.Height = 
			rect_data.Width = Thickness;
	}
	else if(line->start_pos.StartX == line->end_pos.StartX) {  
		//Vertical line
		rect_data.Width =  Thickness ; 
		rect_data.Postion.StartX = line->start_pos.StartX - Thickness/2;
		if( line->start_pos.StartY < line->end_pos.StartY) {
			//From up to down
			rect_data.Postion.StartY = line->start_pos.StartY;
			rect_data.Height =  line->end_pos.StartY - line->start_pos.StartY + 1;
		}
		else{
			//From down to up
			rect_data.Postion.StartY = line->end_pos.StartY;
			rect_data.Height =  line->start_pos.StartY - line->end_pos.StartY + 1;
		}
	}
	else{
		//horizonal line
		rect_data.Height = Thickness ; 
		rect_data.Postion.StartY = line->start_pos.StartY - Thickness/2;
		if( line->start_pos.StartX < line->end_pos.StartX) {
			//From left to right
			rect_data.Postion.StartX = line->start_pos.StartX;
			rect_data.Width =  line->end_pos.StartX - line->start_pos.StartX + 1;
		}
		else{
			//From right to left
			rect_data.Postion.StartX = line->end_pos.StartX;
			rect_data.Width =  line->start_pos.StartX - line->end_pos.StartX + 1;
		}
	}
	// Retangle
	
	BF_Osd_Attr_Get_Draw_Area_Size(disp_no_data, screen_size);
	screen_rect_data.Postion.StartX = 0;
	screen_rect_data.Postion.StartY = 0;
	screen_rect_data.Width  = screen_size[D_BF_OSD_ATTR_INDEX_WIDTH];
	screen_rect_data.Height  = screen_size[D_BF_OSD_ATTR_INDEX_LINES];

	ULONG	osd_layer_address = 0;
	if(BF_Osd_Draw_Comm_Get_Overlap_Rect(screen_rect_data,rect_data,&rect_data) ){
		BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(&rect_data.Postion, &osd_layer_address, disp_no_data);
			// Make its to VGR buffer
#if FJ_OSD_COMM_RGBAXXXX == 2
			// RGBA4444
		OSD_USHORT_INT color_data = (OSD_USHORT_INT)BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.body_color.word);
#else
		OSD_USHORT_INT color_data =(OSD_USHORT_INT)gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].shape.body_color.word;					// RGBA8888
#endif
		BF_Osd_General_Fill_Box_For_GR(color_data, screen_rect_data.Width, (OSD_USHORT_INT*)osd_layer_address, rect_data.Width, rect_data.Height);
	}
	return ret;
}

FJ_ERR_CODE BF_Osd_Shape_Comm_drawslant(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, const FJ_LINE_COORDINATES* line, USHORT Thickness)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
#if 1
	//		   *(x1,y1)
	//	    1 *   *2
//	(x2,y2) *     *(x4,y4)
	//		 *   * 4 
	//		3  *
	//			(x3,y3)

	//Get base line named line 1
	// (x1,y1) = start_pos
	// (x2,y2) = end_pos
	// Line 1's equation
	//Y-y1=k1(X-x1) -> k1=(y1-y2)/(x1-x2)
	float k1 = //Line1's K
		(float)(line->start_pos.StartY  - line->end_pos.StartY)/(float)(line->start_pos.StartX  - line->end_pos.StartX);
	float k2 = -1/k1;//Line2's K

	//CosA = ROOT(1/(1 + Tg*Tg))
	float CosA;
	//SinA = ROOT(1/(1 + Ctg*Ctg))
	float SinA;
	//Define the less y to y2
	T_POS x1y1 = {0, 0};
	T_POS x2y2 = {0, 0};

	T_POS center_x1y1 = {0, 0};
	T_POS center_x2y2 = {0, 0};

	T_POS x3y3 = {0, 0};
	T_POS x4y4 = {0, 0};
	T_POS parallelogram_across_corners_point_start;
	T_POS parallelogram_across_corners_point_end;

    
	SHORT end_down_triangle_y;//Down triangle part's end
	CosA = sqrt((float)1/((float)1 + k1 * k1));
	SinA = sqrt((float)1/((float)1 + k2 * k2));
	//Vertical to the base lline and pass the (x1,y1)
	// Line 2's equation
	//Y-y1=k2(X-x1) -> k2=-/1k1
	if(k1<0){
		//because of Y is minus direction
		SinA *=(-1);
	}

	if(line->start_pos.StartY  > line->end_pos.StartY) {
		center_x1y1.StartX = line->end_pos.StartX;
		center_x1y1.StartY = line->end_pos.StartY;
		center_x2y2.StartX = line->start_pos.StartX;
		center_x2y2.StartY = line->start_pos.StartY;
	}
	else{
		center_x1y1.StartX = line->start_pos.StartX;
		center_x1y1.StartY = line->start_pos.StartY;
		center_x2y2.StartX = line->end_pos.StartX;
		center_x2y2.StartY = line->end_pos.StartY;
	}
	//Get corner position from center position

	x1y1.StartY = rint(center_x1y1.StartY - Thickness * CosA/2);
	x2y2.StartY = rint(center_x2y2.StartY - Thickness * CosA/2);

	x3y3.StartY = (center_x2y2.StartY + (center_x2y2.StartY - x2y2.StartY));
	x4y4.StartY = (center_x1y1.StartY + (center_x1y1.StartY - x1y1.StartY));

	x1y1.StartX = rint(center_x1y1.StartX + Thickness * SinA/2);
	x2y2.StartX = rint(center_x2y2.StartX + Thickness * SinA/2);
	x3y3.StartX = (center_x2y2.StartX * 2 - x2y2.StartX);
	x4y4.StartX = (center_x1y1.StartX * 2 - x1y1.StartX);

	// Line 3's equation
	//Y-y2=k2(X-x2) -> k2=-k1
	// Line 4's equation
	//Y-y3=k1(X-x3) -> k2=-k1
	//Divied the rectagleto 3 area
	//Draw first triangle(Y less side)
	if(x2y2.StartY < x4y4.StartY) {
		//divide Line4 to 2 parts
        end_down_triangle_y = x4y4.StartY;
        parallelogram_across_corners_point_start = x2y2;
        parallelogram_across_corners_point_end = x4y4;

	}
	else{
		//divide Line3 to 2 parts
        end_down_triangle_y = x2y2.StartY;
        //Draw line1 to line4

        parallelogram_across_corners_point_start = x4y4;
        parallelogram_across_corners_point_end = x2y2;
	}
	if(x2y2.StartX < x4y4.StartX) {
		//From Line3 to Line4
		ret |= osd_shape_comm_draw_triangle_from_2lines(disp_no_data, &x3y3, end_down_triangle_y, k2);
	}
	else{
		//From Line4 to Line3
		ret |= osd_shape_comm_draw_triangle_from_2lines(disp_no_data, &x3y3, end_down_triangle_y, k1);
	}
            //Draw parallelogram
    if (x4y4.StartY < x2y2.StartY)
    {
		ret |= osd_shape_com_draw_parallelogram(disp_no_data, &parallelogram_across_corners_point_start, &parallelogram_across_corners_point_end, k1);
	}
	else{
		//The thickess is bigger than line's length
		ret |= osd_shape_com_draw_parallelogram(disp_no_data, &parallelogram_across_corners_point_start, &parallelogram_across_corners_point_end, k2);
	}

	//Draw seccond triangle(Y bigger side)
	if(x2y2.StartX < x4y4.StartX) {
		//From Line1 to Line2
		ret |= osd_shape_comm_draw_triangle_from_2lines(disp_no_data, &x1y1, parallelogram_across_corners_point_start.StartY, k1);
	}
	else{
		//From Line2 to Line1
		ret |= osd_shape_comm_draw_triangle_from_2lines(disp_no_data, &x1y1, parallelogram_across_corners_point_start.StartY, k2);
	}

#else
	USHORT screen_size[2];//[0]: width [1]:height

	T_BF_OSD_DRAW_COMM_WRITE_INFO oc_infor;
	BYTE color_oc;

	FLOAT diagonal_line_length;
	FLOAT cos_x;
	FLOAT sin_y;

	INT32  slant_x;
	INT32  slant_y;
	INT32  side_slant_x;
	INT32  side_slant_y;

	USHORT i;
	USHORT cnt = 0;
	USHORT lineHeight;
	USHORT sideLineHeight;
	USHORT allLineHeight;
	USHORT length;
	//
	//     *  R
	// T *   *
	//	*      *
	// *		*
	//   *     *
	//	   *  * B
	//	 L   *		
	FJ_LINE_COORDINATES topLine;
	FJ_LINE_COORDINATES bottomLine;
	FJ_LINE_COORDINATES leftLine;
	FJ_LINE_COORDINATES rightLine;

	FJ_LINE_COORDINATES drwaLines[D_BF_OSD_GENERAL_OSD_LINES] = {0};
	// Set Top Line
	// Set start as top side line
	if(line->start_pos.StartY < line->end_pos.StartY)
	{
	//     *(x1,y1)
	//    *
	//   *(x2,y2)
		topLine.start_pos.StartX = line->start_pos.StartX;
		topLine.start_pos.StartY = line->start_pos.StartY;
		topLine.end_pos.StartX = line->end_pos.StartX;
		topLine.end_pos.StartY = line->end_pos.StartY;
	}
	else
	{
	//     *(x2,y2)
	//    *
	//   *(x1,y1)
		topLine.start_pos.StartX = line->end_pos.StartX;
		topLine.start_pos.StartY = line->end_pos.StartY;
		topLine.end_pos.StartX = line->start_pos.StartX;
		topLine.end_pos.StartY = line->start_pos.StartY;
	}
	//   * A
	//  *
	// * B
	diagonal_line_length	//length of AB
		= sqrt(((topLine.start_pos.StartX - topLine.end_pos.StartX) * (topLine.start_pos.StartX - topLine.end_pos.StartX)) 
		+ ((topLine.start_pos.StartY - topLine.end_pos.StartY) * (topLine.start_pos.StartY - topLine.end_pos.StartY)));
	//		*
	//	  *
	//	* a
	// -----------
	cos_x = (line->start_pos.StartX - line->end_pos.StartX) / diagonal_line_length;
	sin_y = (line->start_pos.StartY - line->end_pos.StartY) / diagonal_line_length;

	// Set Bottom Line
	bottomLine.start_pos.StartX = BF_Osd_General_Get_Max((SHORT)(topLine.start_pos.StartX + ((Thickness - 1) * sin_y)), 0);
	bottomLine.start_pos.StartY = BF_Osd_General_Get_Max(topLine.start_pos.StartY - ((Thickness - 1) * cos_x), 0);
	bottomLine.end_pos.StartX = BF_Osd_General_Get_Max((SHORT)(topLine.end_pos.StartX + ((Thickness - 1) * sin_y)), 0);
	bottomLine.end_pos.StartY = BF_Osd_General_Get_Max(topLine.end_pos.StartY - ((Thickness - 1) * cos_x), 0);

	if(topLine.start_pos.StartX < topLine.end_pos.StartX)
	{
		// Set Left Line
		leftLine.start_pos = topLine.start_pos;
		leftLine.end_pos = bottomLine.start_pos;
		// Set Right Line
		rightLine.start_pos = topLine.end_pos;
		rightLine.end_pos = bottomLine.end_pos;
	}
	else
	{
		// Set Left Line
		leftLine.start_pos = topLine.end_pos;
		leftLine.end_pos = bottomLine.end_pos;
		// Set Right Line
		rightLine.start_pos = topLine.start_pos;
		rightLine.end_pos = bottomLine.start_pos;
	}

	slant_x = (topLine.start_pos.StartX - topLine.end_pos.StartX);
	slant_y = (topLine.start_pos.StartY - topLine.end_pos.StartY);
	side_slant_x = (leftLine.start_pos.StartX - leftLine.end_pos.StartX);
	side_slant_y = (leftLine.start_pos.StartY - leftLine.end_pos.StartY);

	lineHeight    = topLine.end_pos.StartY - topLine.start_pos.StartY + 1;
	sideLineHeight = leftLine.end_pos.StartY - leftLine.start_pos.StartY + 1;
	allLineHeight  = bottomLine.end_pos.StartY - topLine.start_pos.StartY + 1;
	if(topLine.start_pos.StartX < topLine.end_pos.StartX)
	{
		// TANa<0
		//	*
		//		*
		//			*
		// Set start points
		//Left line's XY 
		for(i = 0; i < sideLineHeight; i++)
		{
			drwaLines[cnt].start_pos.StartY = leftLine.start_pos.StartY + i;
			drwaLines[cnt].start_pos.StartX = side_slant_x * (drwaLines[cnt].start_pos.StartY - leftLine.end_pos.StartY) / side_slant_y + leftLine.end_pos.StartX;
			cnt++;
		}
		//Top line's XY 
		for(i = 1; i < lineHeight; i++)
		{
			drwaLines[cnt].start_pos.StartY = bottomLine.start_pos.StartY + i;
			drwaLines[cnt].start_pos.StartX = slant_x * (drwaLines[cnt].start_pos.StartY - bottomLine.end_pos.StartY) / slant_y + bottomLine.end_pos.StartX;
			cnt++;
		}

		cnt = 0; // reset counter

		// Set end points
		// Bottom line's XY 
		for(i = 0; i < lineHeight; i++)
		{
			drwaLines[cnt].end_pos.StartY = topLine.start_pos.StartY + i;
			drwaLines[cnt].end_pos.StartX = slant_x * (drwaLines[cnt].end_pos.StartY - topLine.end_pos.StartY) / slant_y + topLine.end_pos.StartX;
			cnt++;
		}
		//Right line's XY 
		for(i = 1; i < sideLineHeight; i++)
		{
			drwaLines[cnt].end_pos.StartY = rightLine.start_pos.StartY + i;
			drwaLines[cnt].end_pos.StartX = side_slant_x * (drwaLines[cnt].end_pos.StartY - rightLine.end_pos.StartY) / side_slant_y + rightLine.end_pos.StartX;
			cnt++;
		}
	}
	else
	{
		// TANa>0
		//			*
		//		*
		//	*		
		// Set start points
		for(i = 0; i < lineHeight; i++)
		{
			drwaLines[cnt].start_pos.StartY = topLine.start_pos.StartY + i;
			drwaLines[cnt].start_pos.StartX = slant_x * (drwaLines[cnt].start_pos.StartY - topLine.end_pos.StartY) / slant_y + topLine.end_pos.StartX;
			cnt++;
		}
		for(i = 1; i < sideLineHeight; i++)
		{
			drwaLines[cnt].start_pos.StartY = leftLine.start_pos.StartY + i;
			drwaLines[cnt].start_pos.StartX = side_slant_x * (drwaLines[cnt].start_pos.StartY - leftLine.end_pos.StartY) / side_slant_y + leftLine.end_pos.StartX;
			cnt++;
		}

		cnt = 0; // reset counter

		// Set end points
		for(i = 0; i < sideLineHeight; i++)
		{
			drwaLines[cnt].end_pos.StartY = rightLine.start_pos.StartY + i;
			drwaLines[cnt].end_pos.StartX = side_slant_x * (drwaLines[cnt].end_pos.StartY - rightLine.end_pos.StartY) / side_slant_y + rightLine.end_pos.StartX;
			cnt++;
		}
		for(i = 1; i < lineHeight; i++)
		{
			drwaLines[cnt].end_pos.StartY = bottomLine.start_pos.StartY + i;
			drwaLines[cnt].end_pos.StartX = slant_x * (drwaLines[cnt].end_pos.StartY - bottomLine.end_pos.StartY) / slant_y + bottomLine.end_pos.StartX;
			cnt++;
		}
	}

	switch(bDisplayID) {
		case E_OSDLayer_LCD_OSD0:       // FALL THROUGH
		case E_OSDLayer_LCD_OSD1:       // FALL THROUGH
		case E_OSDLayer_HDMI_OSD1:
		case E_OSDLayer_HDMI_OSD0:
		{
			// Draw to VGRch.
			if((bDisplayID == E_OSDLayer_LCD_OSD0) || (bDisplayID == E_OSDLayer_LCD_OSD1))
			{
				OSD_USHORT_INT line_color = (OSD_USHORT_INT)gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_LCD_MIPI].shape.body_color.word;
			}
			else{
				OSD_USHORT_INT line_color = (OSD_USHORT_INT)gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].shape.body_color.word;
			
			//Get campus size
			BF_Osd_Attr_Get_Screen_Size(bDisplayID, screen_size);
			// Pointer initialize
			for(i = 0; i < allLineHeight; i++)
			{
				ULONG	osd_layer_address = 0;
				INT32 j;
				OSD_USHORT_INT* gr_address ;
				BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(&drwaLines[i].start_pos,
													 screen_size[D_BF_OSD_ATTR_INDEX_WIDTH],
													 &osd_layer_address,
													 bDisplayID);

				gr_address = (OSD_USHORT_INT*)osd_layer_address;
				length = drwaLines[i].end_pos.StartX - drwaLines[i].start_pos.StartX + 1;
				for(j = 0; j < length; j++){

					*gr_address = line_color;
					gr_address++;

				}
			}
			break;
		}
		default:
			ret = FJ_ERR_OSD_LAYER;
			break;

	}
#endif
	return ret;
}
