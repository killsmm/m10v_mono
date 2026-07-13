/**
 * @file		osd_draw_comm.c
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"
#include "play_common.h"
#include "osd_config_comm.h"
#include "osd_draw_comm.h"
#include "osd_image_comm.h"
#include "osd_attr.h"
#include "osd_blend_table.h"
#include "osd_font_table.h"
#include "sdram_map_cache_define.h"
/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Enumeration															*/

/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Structure															*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Data															*/
/*----------------------------------------------------------------------*/
#ifdef CO_OSD_FONT_SIZE_IS_VGA
/** Font table */
extern const UINT32 gOsd_Font_Tbl[95][17];

/** gOsd_Font_Tbl's size. [Width,Height] */
extern const UINT32 gOsd_Font_Tbl_size[95][2];
#else
/** Font table */
const unsigned short gOsd_Font_1216_Tbl[95][16];
#endif // CO_OSD_FONT_SIZE_IS_VGA

T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA gBF_Osd_Draw_Comm_Showing_Area_Inf[E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_END][E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_END] = 
{
	//HMDI
	{
		//GR0
		{
			//area_size[OSD_DRAW_AREA_NUM]
			{
				{ { 0,0 }, D_BF_OSD_GENERAL_OSD_WIDTH,D_BF_OSD_GENERAL_OSD_LINES },//0
				{ { 0,0 }, 0,0 },//1
				{ { 0,0 }, 0,0 },//2
				{ { 0,0 }, 0,0 },//3
				{ { 0,0 }, 0,0 },//4
				{ { 0,0 }, 0,0 },//5
				{ { 0,0 }, 0,0 },//6
				{ { 0,0 }, 0,0 },//7
				{ { 0,0 }, 0,0 },//8
				{ { 0,0 }, 0,0 },//9
			},
			//enable_area
			0x0001,
			//vaild_area_num
			1,
		},
		//GR1
		{
			//area_size[OSD_DRAW_AREA_NUM]
			{
				{ { 0,0 }, D_BF_OSD_GENERAL_OSD_WIDTH,D_BF_OSD_GENERAL_OSD_LINES },//0
				{ { 0,0 }, 0,0 },//1
				{ { 0,0 }, 0,0 },//2
				{ { 0,0 }, 0,0 },//3
				{ { 0,0 }, 0,0 },//4
				{ { 0,0 }, 0,0 },//5
				{ { 0,0 }, 0,0 },//6
				{ { 0,0 }, 0,0 },//7
				{ { 0,0 }, 0,0 },//8
				{ { 0,0 }, 0,0 },//9
			},
			//enable_area
			0x0001,
			//vaild_area_num
			1
		}
	},
	//LCD
	{
		//GR0
		{
			//area_size[OSD_DRAW_AREA_NUM]
			{
				{ { 0,0 }, D_BF_OSD_GENERAL_OSD_WIDTH,D_BF_OSD_GENERAL_OSD_LINES },//0
				{ { 0,0 }, 0,0 },//1
				{ { 0,0 }, 0,0 },//2
				{ { 0,0 }, 0,0 },//3
				{ { 0,0 }, 0,0 },//4
				{ { 0,0 }, 0,0 },//5
				{ { 0,0 }, 0,0 },//6
				{ { 0,0 }, 0,0 },//7
				{ { 0,0 }, 0,0 },//8
				{ { 0,0 }, 0,0 },//9
			},
			//enable_area
			0x0001,
			//vaild_area_num
			1
		},
		{
			//area_size[OSD_DRAW_AREA_NUM]
			{
				{ { 0,0 }, D_BF_OSD_GENERAL_OSD_WIDTH,D_BF_OSD_GENERAL_OSD_LINES },//0
				{ { 0,0 }, 0,0 },//1
				{ { 0,0 }, 0,0 },//2
				{ { 0,0 }, 0,0 },//3
				{ { 0,0 }, 0,0 },//4
				{ { 0,0 }, 0,0 },//5
				{ { 0,0 }, 0,0 },//6
				{ { 0,0 }, 0,0 },//7
				{ { 0,0 }, 0,0 },//8
				{ { 0,0 }, 0,0 },//9
			},
			//enable_area
			0x0001,
			//vaild_area_num
			1
		}
	}
};

// OSD attribute for FJ_SetGDA()
T_BF_OSD_DRAW_COMM_GR_GDA_ATTRIBUTE gBF_Osd_Draw_Comm_GDA_Attr = {
	{
		{0, 0}			// T_POS left_top_pos
	},	// common
	{
		{0x00B2AB00},	// T_IconColor outline_color
		{0x004C54FF}	// T_IconColor body_color
	}	// shape
};

// OSD attribute for FJ_SetOSDDisplayAttr()
T_BF_OSD_DRAW_COMM_OSD_ATTRIBUTE gBF_Osd_Draw_Comm_Osd_Attr[E_FJ_DISP_COMM_BLOCK_NUM] = {
	{	.common.left_top_pos.StartX = 0,
		.common.left_top_pos.StartY = 0,

		.disp.interface = E_FJ_DISP_IFS_NONE,

#if (FJ_OSD_COMM_OC_SOFT_LAYER_ON == 1)
		.color.skip[E_BF_OSD_DRAW_COMM_SKIP_COLOR_ENABLE] = 0,
		.color.skip[E_BF_OSD_DRAW_COMM_SKIP_COLOR_PALETTE_INDEX] = 0,// BYTE skip[E_BF_OSD_DRAW_COMM_SKIP_COLOR_END] (not Skip)
#else
		.color.skip[E_BF_OSD_DRAW_COMM_SKIP_COLOR_ENABLE] = 1,
		.color.skip[E_BF_OSD_DRAW_COMM_SKIP_COLOR_PALETTE_INDEX] = 0,// BYTE skip[E_BF_OSD_DRAW_COMM_SKIP_COLOR_END] (not Skip)
#endif

		.shape.outline_color.word = 0x0cB2AB00,		// T_IconColor outline_color
		.shape.body_color.word = 0x044C54FF,		// T_IconColor body_color
		.shape.outline_size.StartX = 2,// T_POS outline_size X
		.shape.outline_size.StartY = 2,// T_POS outline_size Y
		.shape.round_rect_radius = 4, // USHORT round_rect_radius
		.shape.Donut_Rect_frame_size = 1,				// USHORT donut

		.text.body_color.word = 	0x00000001,		// T_IconColor body_color
		.text.space_size.StartX = 0,			// T_POS space_size
		.text.space_size.StartY = 0,			// T_POS space_size
		.text.text_size.StartX =0,// T_POS text_size X
		.text.text_size.StartY =0,// T_POS text_size Y

		.histogram.lcd.pen_color.word = D_BF_OSD_DRAW_COMM_HIST_BOX_PEN_COLOR,		// T_IconColor pen_color
		.histogram.lcd.graph_color.word = D_BF_OSD_DRAW_COMM_HIST_GRAPH_COLOR,		// T_IconColor graph_color
		.histogram.lcd.back_color.word = D_BF_OSD_DRAW_COMM_HIST_BACK_COLOR,		// T_IconColor back_color

		.histogram.hdmi.pen_color.word = D_BF_OSD_DRAW_COMM_HIST_BOX_PEN_COLOR,		// T_IconColor pen_color
		.histogram.hdmi.graph_color.word = D_BF_OSD_DRAW_COMM_HIST_GRAPH_COLOR,		// T_IconColor graph_color
		.histogram.hdmi.back_color.word = D_BF_OSD_DRAW_COMM_HIST_BACK_COLOR,		// T_IconColor back_color

		.hdmi.format =	E_HD_RESOLUTION_1080I_60H,				// BYTE format
		.hdmi.scan =	D_BF_OSD_ATTR_HDMI_INTERLACE,	// BYTE scan
		.hdmi.main.global_width = 1920,					// USHORT global_width
		.hdmi.main.width = 1920,					// USHORT width
		.hdmi.main.lines = 1080,					// USHORT line
		.hdmi.main.pGet_SDRAM_HDch = NULL,
		.hdmi.main.pChange_SDRAM_HDch = NULL,
		.hdmi.osd[E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0].width = D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_WIDTH_HD,			// USHORT width
		.hdmi.osd[E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0].lines = D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_LINES_HD,			// USHORT line
		.hdmi.osd[E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1].width = D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_WIDTH_HD,			// USHORT width
		.hdmi.osd[E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1].lines = D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_LINES_HD,			// USHORT line

		.soft_layer.gr.blend_ratio = {19, 19, 19},	// BYTE blend_ratio[3]
	},
	{	.common.left_top_pos.StartX = 0,
		.common.left_top_pos.StartY = 0,

		.disp.interface = E_FJ_DISP_IFS_NONE,

#if (FJ_OSD_COMM_OC_SOFT_LAYER_ON == 1)
		.color.skip[E_BF_OSD_DRAW_COMM_SKIP_COLOR_ENABLE] = 0,
		.color.skip[E_BF_OSD_DRAW_COMM_SKIP_COLOR_PALETTE_INDEX] = 0,// BYTE skip[E_BF_OSD_DRAW_COMM_SKIP_COLOR_END] (not Skip)
#else
		.color.skip[E_BF_OSD_DRAW_COMM_SKIP_COLOR_ENABLE] = 1,
		.color.skip[E_BF_OSD_DRAW_COMM_SKIP_COLOR_PALETTE_INDEX] = 0,// BYTE skip[E_BF_OSD_DRAW_COMM_SKIP_COLOR_END] (not Skip)
#endif

		.shape.outline_color.word = 0x0cB2AB00,		// T_IconColor outline_color
		.shape.body_color.word = 0x044C54FF,		// T_IconColor body_color
		.shape.outline_size.StartX = 2,// T_POS outline_size X
		.shape.outline_size.StartY = 2,// T_POS outline_size Y
		.shape.round_rect_radius = 4, // USHORT round_rect_radius
		.shape.Donut_Rect_frame_size = 1,				// USHORT donut

		.text.body_color.word = 	0x00000001,		// T_IconColor body_color
		.text.space_size.StartX = 0,			// T_POS space_size
		.text.space_size.StartY = 0,			// T_POS space_size
		.text.text_size.StartX =0,// T_POS text_size X
		.text.text_size.StartY =0,// T_POS text_size Y

		.histogram.lcd.pen_color.word = D_BF_OSD_DRAW_COMM_HIST_BOX_PEN_COLOR,		// T_IconColor pen_color
		.histogram.lcd.graph_color.word = D_BF_OSD_DRAW_COMM_HIST_GRAPH_COLOR,		// T_IconColor graph_color
		.histogram.lcd.back_color.word = D_BF_OSD_DRAW_COMM_HIST_BACK_COLOR,		// T_IconColor back_color

		.histogram.hdmi.pen_color.word = D_BF_OSD_DRAW_COMM_HIST_BOX_PEN_COLOR,		// T_IconColor pen_color
		.histogram.hdmi.graph_color.word = D_BF_OSD_DRAW_COMM_HIST_GRAPH_COLOR,		// T_IconColor graph_color
		.histogram.hdmi.back_color.word = D_BF_OSD_DRAW_COMM_HIST_BACK_COLOR,		// T_IconColor back_color

		.hdmi.format =	E_HD_RESOLUTION_1080I_60H,				// BYTE format
		.hdmi.scan =	D_BF_OSD_ATTR_HDMI_INTERLACE,	// BYTE scan
		.hdmi.main.global_width = 1920,					// USHORT global_width
		.hdmi.main.width = 1920,					// USHORT width
		.hdmi.main.lines = 1080,					// USHORT line
		.hdmi.main.pGet_SDRAM_HDch = NULL,
		.hdmi.main.pChange_SDRAM_HDch = NULL,
		.hdmi.osd[E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0].width = D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_WIDTH_HD,			// USHORT width
		.hdmi.osd[E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0].lines = D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_LINES_HD,			// USHORT line
		.hdmi.osd[E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1].width = D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_WIDTH_HD,			// USHORT width
		.hdmi.osd[E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1].lines = D_BF_OSD_GENERAL_RESOLUTION_HGR_INPUT_LINES_HD,			// USHORT line

		.soft_layer.gr.blend_ratio = {19, 19, 19},	// BYTE blend_ratio[3]
	}
};

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
///////////////////////////////////////////////////
//  Following are BASEFW_BOOT_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_BOOT_CODE"
FJ_ERR_CODE BF_Osd_Draw_Comm_Set_Comm_Display_Attr(E_OSDLayer bDisplayID, E_FJ_OSD_ATTR_TYPE Attr,
									   INT32 wParam1, INT32 wParam2, INT32 wParam3, INT32 wParam4)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	E_FJ_DISP_COMM_BLOCK block = E_FJ_DISP_COMM_BLOCK_0;

	/* change layer id to block number. */
	if( ( bDisplayID & E_OSDLayer_HDMI ) == E_OSDLayer_HDMI ){
		block = E_FJ_DISP_COMM_BLOCK_0;
	}
	else{
		block = E_FJ_DISP_COMM_BLOCK_1;
	}

	switch (Attr) {
		case E_FJ_OSD_ATTR_CENTER:
			// Set standard coordinates of the OSD drawing.
			gBF_Osd_Draw_Comm_Osd_Attr[block].common.left_top_pos.StartX = wParam1;    // X coordinates.
			gBF_Osd_Draw_Comm_Osd_Attr[block].common.left_top_pos.StartY = wParam2;    // Y coordinates.
			break;

		case E_FJ_OSD_ATTR_BRUSH_COLOR:
			// Set body color of shape.
			gBF_Osd_Draw_Comm_Osd_Attr[block].shape.body_color.word = wParam1;
			break;

		case E_FJ_OSD_ATTR_PEN_COLOR:
			// Set frame color of shape.
			gBF_Osd_Draw_Comm_Osd_Attr[block].shape.outline_color.word = wParam1;
			break;

		case E_FJ_OSD_ATTR_TEXT_SPACE:
			// Set space between the characters.
			gBF_Osd_Draw_Comm_Osd_Attr[block].text.space_size.StartX = wParam1;    // Width
			gBF_Osd_Draw_Comm_Osd_Attr[block].text.space_size.StartY = wParam2;    // Height
			break;

		case E_FJ_OSD_ATTR_FRAME_THICKNESS:
			// Set thickness of frame.
			gBF_Osd_Draw_Comm_Osd_Attr[block].shape.outline_size.StartX = (wParam1);
			gBF_Osd_Draw_Comm_Osd_Attr[block].shape.outline_size.StartY = (wParam2);
			break;

		case E_FJ_OSD_ATTR_HISTO_PEN_COLOR:
			// Set frame color of histogram.
			if (BF_Osd_Draw_Comm_Is_OSD1_Layer(bDisplayID) ||
				BF_Osd_Draw_Comm_Is_OSD0_Layer(bDisplayID)) {
				// OSD
				gBF_Osd_Draw_Comm_Osd_Attr[block].histogram.lcd.pen_color.word = wParam1;
			}
			else if(BF_Osd_Draw_Comm_Is_HDMI_OSD1_Layer(bDisplayID) ||
					BF_Osd_Draw_Comm_Is_HDMI_OSD0_Layer(bDisplayID)) {
				// HDMI
				gBF_Osd_Draw_Comm_Osd_Attr[block].histogram.hdmi.pen_color.word = wParam1;
			}
			else {
				// Layer error.
				ret = FJ_ERR_OSD_LAYER;
			}
			break;

		case E_FJ_OSD_ATTR_HISTO_GRAF_COLOR:
			// Set graph color of histogram.
			if (BF_Osd_Draw_Comm_Is_OSD1_Layer(bDisplayID) ||
				BF_Osd_Draw_Comm_Is_OSD0_Layer(bDisplayID)) {
				// OSD
				gBF_Osd_Draw_Comm_Osd_Attr[block].histogram.lcd.graph_color.word = wParam1;
			}
			else if(BF_Osd_Draw_Comm_Is_HDMI_OSD1_Layer(bDisplayID) ||
					BF_Osd_Draw_Comm_Is_HDMI_OSD0_Layer(bDisplayID)) {
				// HDMI
				gBF_Osd_Draw_Comm_Osd_Attr[block].histogram.hdmi.graph_color.word = wParam1;
			}
			else {
				// Layer error.
				ret = FJ_ERR_OSD_LAYER;
			}
			break;

		case E_FJ_OSD_ATTR_HISTO_GRAF_BACK_COLOR:
			// Set background color of histogram.
			if (BF_Osd_Draw_Comm_Is_OSD1_Layer(bDisplayID) ||
				BF_Osd_Draw_Comm_Is_OSD0_Layer(bDisplayID)) {
				// OSD
				gBF_Osd_Draw_Comm_Osd_Attr[block].histogram.lcd.back_color.word = wParam1;
			}
			else if(BF_Osd_Draw_Comm_Is_HDMI_OSD1_Layer(bDisplayID) ||
					BF_Osd_Draw_Comm_Is_HDMI_OSD0_Layer(bDisplayID)) {
				// HDMI
				gBF_Osd_Draw_Comm_Osd_Attr[block].histogram.hdmi.back_color.word = wParam1;
			}
			else {
				// Layer error.
				ret = FJ_ERR_OSD_LAYER;
			}
			break;

		case E_FJ_OSD_ATTR_REPLACE_COLOR:
			// Set replace color.
			gBF_Osd_Draw_Comm_Osd_Attr[block].color.replace[E_BF_OSD_DRAW_COMM_REPLACE_COLOR_FROM].word = wParam1;
			gBF_Osd_Draw_Comm_Osd_Attr[block].color.replace[E_BF_OSD_DRAW_COMM_REPLACE_COLOR_TO].word  = wParam2;
			break;

		case E_FJ_OSD_ATTR_ROUND_RECT_RADIUS:
			// Set radius of the circle located in the corner of RoundRect.
			if (wParam1 > 2) {
				// Radius is normal.
				gBF_Osd_Draw_Comm_Osd_Attr[block].shape.round_rect_radius = (USHORT)wParam1;
			}
			else {
				// Parameter error.
				ret = FJ_ERR_NG_INPUT_PARAM;
			}
			break;

		case E_FJ_OSD_ATTR_DONUT_FRAME_SIZE:
			gBF_Osd_Draw_Comm_Osd_Attr[block].shape.Donut_Rect_frame_size = wParam1;
			break;

		default:
			// Parameter error.
			ret = FJ_ERR_NG_INPUT_PARAM;
			break;
	}
	return ret;
}

FJ_ERR_CODE BF_Osd_Draw_Comm_Get_Block_GR(E_OSDLayer layer_id, T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	disp_no_data->layer_name = layer_id;
	switch (layer_id){
		case E_OSDLayer_LCD_OSD0:
			disp_no_data->layer = E_IM_DISP_SEL_LAYER_OSD_0;
			disp_no_data->disp_block_no = E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_LCD;
			disp_no_data->gr_no = E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0;
			break;
			
		case E_OSDLayer_LCD_OSD1:
			disp_no_data->layer = E_IM_DISP_SEL_LAYER_OSD_1;
			disp_no_data->disp_block_no = E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_LCD;
			disp_no_data->gr_no = E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1;
			break;
			
		case E_OSDLayer_HDMI_OSD0:
			disp_no_data->layer = E_IM_DISP_SEL_LAYER_OSD_0;
			disp_no_data->disp_block_no = E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_HDMI;
			disp_no_data->gr_no = E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0;
			break;
			
		case E_OSDLayer_HDMI_OSD1:
			disp_no_data->layer = E_IM_DISP_SEL_LAYER_OSD_1;
			disp_no_data->disp_block_no = E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_HDMI;
			disp_no_data->gr_no = E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1;
			break;
			
		default:
			// Parameter error.
			ret = FJ_ERR_NG_INPUT_PARAM;
			break;
	}
	return ret;
}

VOID BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, T_POS* positon)
{
	T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;
	draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data->disp_block_no][disp_no_data->gr_no];

	positon->StartX += gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].common.left_top_pos.StartX;
	positon->StartY += gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].common.left_top_pos.StartY;

	positon->StartX -= draw_area_inf->area_size[disp_no_data->draw_area].Postion.StartX;
	positon->StartY -= draw_area_inf->area_size[disp_no_data->draw_area].Postion.StartY;
}

BOOL BF_Osd_Draw_Comm_Is_OSD1_Layer(E_OSDLayer layer_id)
{
	BOOL is_osd11_layer;

	switch (layer_id) {
		case E_OSDLayer_LCD_OSD1:
			// OSD0
			is_osd11_layer = TRUE;
			break;

		default:
			// Other layer
			is_osd11_layer = FALSE;
			break;
	}
	return is_osd11_layer;
}

BOOL BF_Osd_Draw_Comm_Is_OSD0_Layer(E_OSDLayer layer_id)
{
	BOOL is_osd0_layer;

	switch (layer_id) {
		case E_OSDLayer_LCD_OSD0:
			// OSD1
			is_osd0_layer = TRUE;
			break;

		default:
			// Other layer
			is_osd0_layer = FALSE;
			break;
	}
	return is_osd0_layer;
}

BOOL BF_Osd_Draw_Comm_Is_HDMI_OSD1_Layer(E_OSDLayer layer_id)
{
	BOOL is_hosd1_layer;

	switch (layer_id) {
		case E_OSDLayer_HDMI_OSD1:
			// HDMI OSD1
			is_hosd1_layer = TRUE;
			break;

		default:
			// Other layer
			is_hosd1_layer = FALSE;
			break;
	}
	return is_hosd1_layer;
}

BOOL BF_Osd_Draw_Comm_Is_HDMI_OSD0_Layer(E_OSDLayer layer_id)
{
	BOOL is_hosd0_layer;

	switch (layer_id) {
		case E_OSDLayer_HDMI_OSD0:
			// HDMI OSD0
			is_hosd0_layer = TRUE;
			break;

		default:
			// Other layer
			is_hosd0_layer = FALSE;
			break;
	}
	return is_hosd0_layer;
}

///////////////////////////////////////////////////
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_COMM_CODE"
VOID BF_Osd_Draw_Comm_Print_String_GR(ULONG vgr_address, UINT32 color[FJ_OSD_PRINT_COLOR_INDEX_END], 
									const CHAR* string, UINT32 screen_width, UINT32 screen_height, T_POS postion)
{
	INT32 i, j;
	OSD_USHORT_INT *write_ptr = (OSD_USHORT_INT*)vgr_address; 	/**< VGR MAP Write pointer  */
	const UINT32 *font_ptn;
	INT32 all_char_widht = postion.StartX; 

	while( *string != '\0'){
		//string loop
		INT32 char_height = gOsd_Font_Tbl_size[((*string)- 0x20)][1];
		INT32 char_widht = gOsd_Font_Tbl_size[((*string)- 0x20)][0];

		all_char_widht += char_widht;
		if(screen_width < all_char_widht || screen_height < postion.StartY + char_height){
			break;
		}
		
		for(j = 0; j < char_height ; j++){
			//height of string loop
			UINT32 pixcel_bit = 0x80000000;
			font_ptn = &gOsd_Font_Tbl[((*string)- 0x20)][j];
			for(i = 0;  i < char_widht; i++){
			//width of string loop
				if(pixcel_bit & (*font_ptn)) {
					//Font data body
					*(write_ptr + i) = (OSD_USHORT_INT)color[FJ_OSD_PRINT_COLOR_INDEX_TEXT];

				}
				else{
					//Back 
					*(write_ptr + i) = (OSD_USHORT_INT)color[FJ_OSD_PRINT_COLOR_INDEX_BACK];
				}
				pixcel_bit >>= 1;
			}
			write_ptr += screen_width;
		}
		vgr_address += (char_widht * sizeof(OSD_USHORT_INT));
		write_ptr = (OSD_USHORT_INT*)vgr_address;
		string++;
	}
}

VOID BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(T_POS const* const pos, ULONG* osd_layer_address, const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data)
{
	T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;
	draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data->disp_block_no][disp_no_data->gr_no];

	USHORT draw_area_width = draw_area_inf->area_size[disp_no_data->draw_area].Width;
	*osd_layer_address = M_SDRAM_MAP_COMMON_ADR_PHY2LOG(Sdram_Map_Osd_Get_OSD_Chg_Addr(disp_no_data));
	(*osd_layer_address) += ((pos->StartX + draw_area_width * pos->StartY) * FJ_OSD_COMM_RGBAXXXX);
}

E_BF_OSD_DRAW_COMM_MODE_GROUP4HDMI BF_Osd_Draw_Comm_Get_Mode_Group(FJ_MODE mode)
{
	E_BF_OSD_DRAW_COMM_MODE_GROUP4HDMI ret;
	switch (mode) {
		case FJ_MODE_STORE_IMAGE_FLASH:		// FALL THROUGH
		case FJ_MODE_CAPTURE_H264_AUDIO:	// FALL THROUGH
		case FJ_MODE_CAPTURE_H264:			// FALL THROUGH
		case FJ_MODE_CAPTURE_SEQ:			// FALL THROUGH
		case FJ_MODE_CAPTURE_AEB:			// FALL THROUGH
		case FJ_MODE_CAPTURE:				// FALL THROUGH
		case FJ_MODE_CAPTURE_NIGHT:			// FALL THROUGH
		case FJ_MODE_CALIBRATION:			// FALL THROUGH
		case FJ_MODE_VIEW:					// FALL THROUGH
		case FJ_MODE_CAPTURE_DRAFT:			// FALL THROUGH
		case FJ_MODE_PREVIEW_ZOOM:			// FALL THROUGH
		case FJ_MODE_MOVIE_VIEW:			// FALL THROUGH
		case FJ_MODE_PANORAMA_VIEW:			// FALL THROUGH
		case FJ_MODE_HIGHSPEED:
			ret = E_BF_OSD_DRAW_COMM_MODE_GROUP4HDMI_VIEW;
			break;

		case FJ_MODE_PLAYBACK:				// FALL THROUGH
		case FJ_MODE_PLAYBACK_THUMB:		// FALL THROUGH
		case FJ_MODE_DISP_IMAGE_MAIN:		// FALL THROUGH
		case FJ_MODE_DISP_IMAGE_THUMB:		// FALL THROUGH
		case FJ_MODE_DISP_THUMB_DRAW:		// FALL THROUGH
		case FJ_MODE_DISP_THUMBINDEX:		// FALL THROUGH
		case FJ_MODE_DISP_MONCLS1_MAIN: 	// FALL THROUGH
		case FJ_MODE_DISP_MONCLS1_THUMB:	// FALL THROUGH
		case FJ_MODE_DISP_MONCLS2_MAIN: 	// FALL THROUGH
		case FJ_MODE_DISP_MONCLS2_THUMB: 	// FALL THROUGH
		case FJ_MODE_RESTORE_MAIN_DECODE:	// FALL THROUGH
		case FJ_MODE_MENU:
			ret = E_BF_OSD_DRAW_COMM_MODE_GROUP4HDMI_PB_MENU;
			break;

		default:
			// The memory used by HDMI don't used at above mention mode
			ret = E_BF_OSD_DRAW_COMM_MODE_GROUP4HDMI_OTHER;
			break;
	}
	gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.main.pGet_SDRAM_HDch = Sdram_Map_Common_Get_HDMI_HD;
	gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.main.pChange_SDRAM_HDch = Sdram_Map_Common_Change_HDMI_HD;
	return ret;
}

BOOL BF_Osd_Draw_Comm_Get_Overlap_Rect(T_Rect rect1, T_Rect rect2, T_Rect* overlap_rect)
{
	BOOL	overlap = TRUE;
	SHORT	rect1_right_X  = rect1.Postion.StartX + rect1.Width -1;
	SHORT	rect2_right_X  = rect2.Postion.StartX + rect2.Width -1;
	SHORT	rect1_bottom_Y = rect1.Postion.StartY + rect1.Height -1;
	SHORT	rect2_bottom_Y = rect2.Postion.StartY + rect2.Height -1;
	SHORT	right, bottom;

	if ((rect1.Postion.StartX > rect2_right_X)  ||
		(rect1_right_X < rect2.Postion.StartX)  ||
		(rect1_bottom_Y < rect2.Postion.StartY) ||
		(rect1.Postion.StartY > rect2_bottom_Y)) {
		overlap = FALSE;
	}
	else {

		if ((rect1_right_X <= rect2_right_X) && (rect1_right_X >= rect2.Postion.StartX)) {
			//      +---------+
			//      |  rect2  |
			//      |         |
			// --------+
			//  rect1  |
			right = rect1_right_X;
		}
		else {
			//      +---------+
			//      |  rect2  |
			//      |         |
			// -----------------+
			//  rect1           |
			right = rect2_right_X;
		}

		if ((rect1.Postion.StartX <= rect2_right_X) && (rect1.Postion.StartX >= rect2.Postion.StartX)) {
			//      +---------+
			//      |  rect2  |
			//      |         |
			//         +---------
			//         | rect1
			overlap_rect->Postion.StartX = rect1.Postion.StartX;
		}
		else {
			//      +---------+
			//      |  rect2  |
			//      |         |
			//    +---------
			//    | rect1
			overlap_rect->Postion.StartX = rect2.Postion.StartX;
		}

		if ((rect1_bottom_Y <= rect2_bottom_Y) && (rect1_bottom_Y >= rect2.Postion.StartY)) {
			//       +--------+
			//       |   rect2|
			// rect1 | |      |
			// --------+      |
			//       |
			bottom = rect1_bottom_Y;
		}
		else {
			//       +--------+
			//       |   rect2|
			//       |        |
			// rect1 +-|------+
			// --------+
			bottom = rect2_bottom_Y;
		}

		if ((rect1.Postion.StartY <= rect2_bottom_Y) && (rect1.Postion.StartY >= rect2.Postion.StartY)) {
			//      +---------+
			//      |  rect2  |
			//      |         |
			//         +---------
			//         | rect1
			overlap_rect->Postion.StartY = rect1.Postion.StartY;
		}
		else {
			//         +---------
			//      +--|------+
			//      |  | rect2|
			//      |  |      |
			//         | rect1
			overlap_rect->Postion.StartY = rect2.Postion.StartY;
		}
		overlap_rect->Width = right - overlap_rect->Postion.StartX + 1;
		overlap_rect->Height = bottom - overlap_rect->Postion.StartY + 1;
	}
	return overlap;
}

BOOL BF_Osd_Draw_Comm_Is_Rect_At_LCD_Inside(const USHORT window_size[2], T_Rect* dispWnd)
{
	BOOL ret = TRUE;
	if (dispWnd->Postion.StartX < 0) {
		dispWnd->Width += dispWnd->Postion.StartX;
		dispWnd->Postion.StartX = 0;
		dispWnd->Width = BF_Osd_General_Get_Min(dispWnd->Width, window_size[0]);
	}
	else if (dispWnd->Postion.StartX + dispWnd->Width > window_size[0]) {
		dispWnd->Width = window_size[0] - dispWnd->Postion.StartX;
	}
	else {
		// DO NOTHING
	}

	if (dispWnd->Postion.StartY < 0) {
		dispWnd->Height += dispWnd->Postion.StartY;
		dispWnd->Postion.StartY = 0;
		dispWnd->Height = BF_Osd_General_Get_Min(dispWnd->Height, window_size[1]);
	}
	else if (dispWnd->Postion.StartY + dispWnd->Height > window_size[1]) {
		dispWnd->Height = window_size[1] - dispWnd->Postion.StartY;
	}
	else {
		// DO NOTHING
	}

	if ((dispWnd->Width <= 0) || (dispWnd->Height <= 0)) {
		ret = FALSE;
	}
	return ret;
}

BOOL BF_Osd_Draw_Comm_Is_Rect_Inside(T_Rect disp_rect, T_Rect draw_rect)
{
	BOOL	ret = FALSE;
	SHORT	disp_rect_right_X  = disp_rect.Postion.StartX + disp_rect.Width -1;
	SHORT	disp_rect_bottom_Y = disp_rect.Postion.StartY + disp_rect.Height -1;
	SHORT	draw_rect_right_X  = draw_rect.Postion.StartX + draw_rect.Width -1;
	SHORT	draw_rect_bottom_Y = draw_rect.Postion.StartY + draw_rect.Height -1;

	if(disp_rect.Postion.StartX <= draw_rect.Postion.StartX && 
	   disp_rect_right_X >= draw_rect_right_X &&
	   disp_rect.Postion.StartY <= draw_rect.Postion.StartY &&
	   disp_rect_bottom_Y >= draw_rect_bottom_Y){
	   ret = TRUE;
	}
	
	return ret;
}

FJ_ERR_CODE BF_Osd_Draw_Comm_Is_Rect_Draw_Area_Inside(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, 
													 T_POS draw_pos,	/* pgr0339 */
													 SHORT draw_width,	/* pgr0339 */
													 SHORT draw_height	/* pgr0339 */)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	T_BF_OSD_DRAW_COMM_DRAW_SHOW_AREA *draw_area_inf;
	draw_area_inf = &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data->disp_block_no][disp_no_data->gr_no];

	if(disp_no_data->draw_area < draw_area_inf->vaild_area_num){
		T_Rect disp_rect = draw_area_inf->area_size[disp_no_data->draw_area];
		T_Rect draw_rect = {
						.Postion = draw_pos,
						.Width   = draw_width,
						.Height  = draw_height
					};
		if(BF_Osd_Draw_Comm_Is_Rect_Inside(disp_rect, draw_rect) == FALSE) {
			ret = FJ_ERR_NG_INPUT_PARAM;
		}
	}
	else {
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	return ret;
}

FJ_ERR_CODE BF_Osd_Draw_Comm_Check_Draw_Size_Pos_For_YCC420(USHORT uStartX, USHORT uStartY, USHORT uWidth, USHORT uHight)
{
	FJ_ERR_CODE ercd = FJ_ERR_POSITION;

	if (((uStartX & 0x1) == 0) && ((uStartY & 0x1) == 0) && ((uWidth & 0x1) == 0) && ((uHight & 0x1) == 0)) {
		ercd = FJ_ERR_OK;
	}
	return ercd;
}

VOID BF_Osd_Draw_Comm_Blend_Rgba(OSD_USHORT_INT iconCurrentPixel, OSD_USHORT_INT* destCurrrentPixel,
									    BYTE alpha)
{
	UINT32 destAddr;
	UINT32 iconAlpha, destAlpha;
	static OSD_USHORT_INT last_iconCurrentPixel = 0;
	static OSD_USHORT_INT last_destCurrrentPixel = 0;
	static OSD_USHORT_INT last_blend_result = 0;
	static BYTE last_alpha = 0;
	if((iconCurrentPixel == last_iconCurrentPixel) && (last_destCurrrentPixel == *destCurrrentPixel) && (last_alpha == alpha)){
		//It's same as last pixel, so need not to blend
		*destCurrrentPixel = last_blend_result;
	}
	else{
		//Keep the pixel data for next
		last_iconCurrentPixel = iconCurrentPixel;
		last_destCurrrentPixel = *destCurrrentPixel;
		last_alpha = alpha;
#if (FJ_OSD_COMM_RGBAXXXX == 2)
		//Convert to RGBA888 so that blend them
		UINT32 iconCurPixel = BF_Osd_General_TO_RGBA8888_FROM_RGBA4444(iconCurrentPixel);
		UINT32 destCurPixel = BF_Osd_General_TO_RGBA8888_FROM_RGBA4444(*destCurrrentPixel);
#else
		UINT32 iconCurPixel = iconCurrentPixel;
		UINT32 destCurPixel = *destCurrrentPixel;
#endif

		iconAlpha = BF_Osd_General_RGBA_To_A(iconCurPixel);//Upper
		destAlpha = BF_Osd_General_RGBA_To_A(destCurPixel);//Lower
		// Fade icon's alpha
		if(iconAlpha == D_BF_OSD_GENERAL_ALPHA_OPAQUE){
			iconAlpha = alpha;
		}
		else{
			iconAlpha = (iconAlpha * alpha) >> D_BF_OSD_GENERAL_BIT_NUM_PER_PIXEL;
		}

		if(iconAlpha == D_BF_OSD_GENERAL_ALPHA_OPAQUE) {
			//Upper 100% 
			destAddr = iconCurPixel;
		}
		// if iconAlpha == D_BF_OSD_GENERAL_ALPHA_TRANS, this function would not be called
		//else if(iconAlpha == D_BF_OSD_GENERAL_ALPHA_TRANS) {
		//	// Transparent pixel, no handling is required
		//	destAddr = destCurPixel;
		//}
		else if(destAlpha == D_BF_OSD_GENERAL_ALPHA_TRANS){
			//Lower 100% 
			destAddr = (iconCurPixel & (~E_BF_OSD_GENERAL_RGBA_MASK_A)) | (iconAlpha);
		}
		else{
			UINT32 alphaNew, alphaLeft, alphaRight;
			UINT32 destNewPixel;
			UINT32 resultLeft, resultRight;
			
			alphaLeft = iconAlpha;
			alphaRight = ((D_BF_OSD_GENERAL_ALPHA_OPAQUE - iconAlpha) * destAlpha) >> D_BF_OSD_GENERAL_BIT_NUM_PER_PIXEL;
			alphaNew = alphaLeft + alphaRight;

			// XXX some precision loss should div by 255 instead
			// calculate R
			resultLeft = (UINT32)(BF_Osd_General_RGBA_To_R(iconCurPixel) * alphaLeft);
			resultRight = (UINT32)(BF_Osd_General_RGBA_To_R(destCurPixel) * alphaRight);
			destNewPixel = BF_OSD_BLEND_TABLE_BYTE_A_MUL_255_DIV_B((resultLeft + resultRight)>>D_BF_OSD_GENERAL_BIT_NUM_PER_PIXEL, alphaNew);
			// calculate G
			resultLeft = BF_Osd_General_RGBA_To_G(iconCurPixel) * alphaLeft;
			resultRight = BF_Osd_General_RGBA_To_G(destCurPixel) * alphaRight;
			destNewPixel = (destNewPixel << D_BF_OSD_GENERAL_BIT_NUM_PER_PIXEL) + BF_OSD_BLEND_TABLE_BYTE_A_MUL_255_DIV_B((resultLeft + resultRight)>>D_BF_OSD_GENERAL_BIT_NUM_PER_PIXEL, alphaNew);
			// calculate B
			resultLeft = BF_Osd_General_RGBA_To_B(iconCurPixel) * alphaLeft;
			resultRight = BF_Osd_General_RGBA_To_B(destCurPixel) * alphaRight;
			destNewPixel = (destNewPixel << D_BF_OSD_GENERAL_BIT_NUM_PER_PIXEL) + BF_OSD_BLEND_TABLE_BYTE_A_MUL_255_DIV_B((resultLeft + resultRight)>>D_BF_OSD_GENERAL_BIT_NUM_PER_PIXEL, alphaNew);
			
			destAddr = ((destNewPixel << D_BF_OSD_GENERAL_BIT_NUM_PER_PIXEL) + alphaNew);
		}
#if (FJ_OSD_COMM_RGBAXXXX == 2)
		//Convert to RGBA888 so that blend them
		last_blend_result = *destCurrrentPixel = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(destAddr);
#else
		last_blend_result = *destCurrrentPixel = destAddr;
#endif
	}
}

VOID BF_Osd_Draw_Comm_Blend_Box_To_Box(OSD_USHORT_INT* src_addr, USHORT src_gl_width,
									USHORT dst_gl_width, OSD_USHORT_INT* dst_addr,
									USHORT width, USHORT lines
									,BYTE alpha)
{
	INT32 i,j;

	for (i = lines; i>0; i--) {
		for (j = width; j>0; j--) {
#if (FJ_OSD_COMM_RGBAXXXX == 2)
			if((*src_addr) & 0x0f){
				BF_Osd_Draw_Comm_Blend_Rgba(*src_addr, dst_addr, alpha);
			}
#else
			if((*src_addr) & 0xff){
				BF_Osd_Draw_Comm_Blend_Rgba(*src_addr, dst_addr, alpha);
			}
#endif
			dst_addr++;
			src_addr++;
		}
		dst_addr += (dst_gl_width - width);
		src_addr += (src_gl_width - width);
	}
}

///////////////////////////////////////////////////
//  Following are BASEFW_VIEW_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_BOOT_CODE"
UINT32 BF_Osd_Draw_Comm_Get_HD_Draw_Buffer_Addr(T_IM_DISP_IMAGE_ADDR* hd_address)
{
	U_IM_DISP_SIZE hdm_input_size;

	UINT32 display_size = 0;

	if (Im_DISP_Get_Input_Size(E_IM_DISP_HDMI, &hdm_input_size) == D_DDIM_OK) {
		gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.main.global_width = hdm_input_size.size.width;
		gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.main.lines = hdm_input_size.size.lines;
	
		display_size = gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.main.global_width * gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.main.lines;
		E_IM_DISP_BANK bank_no;
		Im_DISP_Get_Main_Bank(E_IM_DISP_HDMI, &bank_no);
		if(bank_no == E_IM_DISP_BANK_10){
			//3 banks
			bank_no = E_IM_DISP_BANK_00;
		}
		else{
			bank_no++;
		}
		 Im_DISP_Get_Addr(E_IM_DISP_HDMI, bank_no, hd_address);
	}

	return display_size;
}

E_BF_OSD_DRAW_COMM_PRINT_COLOR BF_Osd_Draw_Comm_Get_GR_Print_Color(FJ_OSD_PRINT_COLOR oc_color)
{
	E_BF_OSD_DRAW_COMM_PRINT_COLOR  gr_color = E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR_TRANSPARENCY; 
#if FJ_OSD_COMM_RGBAXXXX == 4
	switch(oc_color){
		case FJ_OSD_PRINT_COLOR_BLACK:
			gr_color = E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR8_BLACK;
			break;
		case FJ_OSD_PRINT_COLOR_RED:
			gr_color = E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR8_RED;
			break;
		case FJ_OSD_PRINT_COLOR_YELLOW:
			gr_color = E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR8_YELLOW;
			break;
		default:
			//Do nothing
			break;
	}
#else
	switch(oc_color){
		case FJ_OSD_PRINT_COLOR_BLACK:
			gr_color = E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR4_BLACK;
			break;
		case FJ_OSD_PRINT_COLOR_RED:
			gr_color = E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR4_RED;
			break;
		case FJ_OSD_PRINT_COLOR_YELLOW:
			gr_color = E_BF_OSD_DRAW_COMM_PRINT_COLOR_GR4_YELLOW;
			break;
		default:
			//Do nothing
			break;
	}
#endif
	return gr_color;
}

