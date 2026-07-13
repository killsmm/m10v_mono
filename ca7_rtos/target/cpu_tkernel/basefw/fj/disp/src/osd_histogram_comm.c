/**
 * @file		osd_histogram_comm.c
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
#include "im_xch.h"
#include "osd_histogram_comm.h"
#include "osd_draw_comm.h"

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
// Current histogram buffer
static ULONG gBF_Osd_Histogram_Comm_Current_Hist_Buffer[D_BF_OSD_HISTOGRAM_COMM_DRAW_BUF_SIZE];

// Control data of histogram.
T_BF_OSD_HISTOGRAM_COMM_HIST_ATTR gBF_Osd_Histogram_Comm_LCD_Hist_Attr = {
	{
		D_BF_OSD_HISTOGRAM_COMM_BOX_X,						// USHORT out_pos_x
		D_BF_OSD_HISTOGRAM_COMM_BOX_Y,						// USHORT out_pos_y
		D_BF_OSD_HISTOGRAM_COMM_BOX_WIDTH,					// USHORT out_width
		D_BF_OSD_HISTOGRAM_COMM_BOX_LINES,					// USHORT out_height
		{
			D_BF_OSD_HISTOGRAM_COMM_BOX_V_FRAME_THICKNESS,	// SHORT StartX
			D_BF_OSD_HISTOGRAM_COMM_BOX_H_FRAME_THICKNESS	// SHORT StartY
		}	// T_POS thickness
	},	// box
	{
		(D_BF_OSD_HISTOGRAM_COMM_DRAW_BUF_SIZE / (D_BF_OSD_HISTOGRAM_COMM_BOX_WIDTH - (D_BF_OSD_HISTOGRAM_COMM_BOX_V_FRAME_THICKNESS << 1))),	// 1 column is 4 histogram unit=4
		(D_BF_OSD_HISTOGRAM_COMM_THRESHOLD / (D_BF_OSD_HISTOGRAM_COMM_BOX_LINES - (D_BF_OSD_HISTOGRAM_COMM_BOX_H_FRAME_THICKNESS << 1)))		// 2
	},	// line
	D_BF_OSD_HISTOGRAM_COMM_THRESHOLD,	// UINT32 threshold
	{
		{
			E_FJ_OSD_SHAPE_TYPE_RECT,	// E_FJ_OSD_SHAPE_TYPE wType
			{
				{
					(D_BF_OSD_HISTOGRAM_COMM_BOX_X + D_BF_OSD_HISTOGRAM_COMM_BOX_V_FRAME_THICKNESS),		// SHORT StartX
					(D_BF_OSD_HISTOGRAM_COMM_BOX_Y + D_BF_OSD_HISTOGRAM_COMM_BOX_H_FRAME_THICKNESS)			// SHORT StartY
				}, // T_POS Postion
				(D_BF_OSD_HISTOGRAM_COMM_BOX_WIDTH - (D_BF_OSD_HISTOGRAM_COMM_BOX_V_FRAME_THICKNESS << 1)),	// SHORT Width
				(D_BF_OSD_HISTOGRAM_COMM_BOX_LINES - (D_BF_OSD_HISTOGRAM_COMM_BOX_H_FRAME_THICKNESS << 1))	// SHORT Height
			}	// T_Rect Rect
		},	// T_BF_OSD_SHAPE_COMM_SHAPE_INFO rect_data
		{ D_BF_OSD_DRAW_COMM_HIST_BACK_COLOR },
	},	// T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO display_data
	{
		{
			{
				(D_BF_OSD_HISTOGRAM_COMM_BOX_X + D_BF_OSD_HISTOGRAM_COMM_BOX_V_FRAME_THICKNESS),		// SHORT StartX
				(D_BF_OSD_HISTOGRAM_COMM_BOX_Y + D_BF_OSD_HISTOGRAM_COMM_BOX_H_FRAME_THICKNESS)			// SHORT StartY
			}, // T_POS Postion
			(D_BF_OSD_HISTOGRAM_COMM_BOX_WIDTH - (D_BF_OSD_HISTOGRAM_COMM_BOX_V_FRAME_THICKNESS << 1)),	// SHORT Width
			(D_BF_OSD_HISTOGRAM_COMM_BOX_LINES - (D_BF_OSD_HISTOGRAM_COMM_BOX_H_FRAME_THICKNESS << 1))	// SHORT Height
		},
		0
	},
	0//showing_area_No
};

// Control data of histogram to HOC.
T_BF_OSD_HISTOGRAM_COMM_HIST_ATTR gBF_Osd_Histogram_Comm_HDMI_Hist_Attr = {
	{
		D_BF_OSD_HISTOGRAM_COMM_BOX_X,						// USHORT out_pos_x
		D_BF_OSD_HISTOGRAM_COMM_BOX_Y,						// USHORT out_pos_y
		D_BF_OSD_HISTOGRAM_COMM_BOX_WIDTH,					// USHORT out_width
		D_BF_OSD_HISTOGRAM_COMM_BOX_LINES,					// USHORT out_height
		{
			D_BF_OSD_HISTOGRAM_COMM_BOX_V_FRAME_THICKNESS,	// SHORT StartX
			D_BF_OSD_HISTOGRAM_COMM_BOX_H_FRAME_THICKNESS	// SHORT StartY
		}	// T_POS thickness
	},	// box
	{
		(D_BF_OSD_HISTOGRAM_COMM_DRAW_BUF_SIZE / (D_BF_OSD_HISTOGRAM_COMM_BOX_WIDTH - (D_BF_OSD_HISTOGRAM_COMM_BOX_V_FRAME_THICKNESS << 1))),	// 1 column is 4 histogram unit=4
		(D_BF_OSD_HISTOGRAM_COMM_THRESHOLD / (D_BF_OSD_HISTOGRAM_COMM_BOX_LINES - (D_BF_OSD_HISTOGRAM_COMM_BOX_H_FRAME_THICKNESS << 1)))		// 2
	},	// line
	D_BF_OSD_HISTOGRAM_COMM_THRESHOLD,	// UINT32 threshold
	{
		{
			E_FJ_OSD_SHAPE_TYPE_RECT,	// E_FJ_OSD_SHAPE_TYPE wType
			{
				{
					(D_BF_OSD_HISTOGRAM_COMM_BOX_X + D_BF_OSD_HISTOGRAM_COMM_BOX_V_FRAME_THICKNESS),		// SHORT StartX
					(D_BF_OSD_HISTOGRAM_COMM_BOX_Y + D_BF_OSD_HISTOGRAM_COMM_BOX_H_FRAME_THICKNESS)			// SHORT StartY
				}, // T_POS Postion
				(D_BF_OSD_HISTOGRAM_COMM_BOX_WIDTH - (D_BF_OSD_HISTOGRAM_COMM_BOX_V_FRAME_THICKNESS << 1)),	// SHORT Width
				(D_BF_OSD_HISTOGRAM_COMM_BOX_LINES - (D_BF_OSD_HISTOGRAM_COMM_BOX_H_FRAME_THICKNESS << 1))	// SHORT Height
			}	// T_Rect Rect
		},	// T_BF_OSD_SHAPE_COMM_SHAPE_INFO rect_data
		{ D_BF_OSD_DRAW_COMM_HIST_BACK_COLOR },
	},	// T_BF_OSD_SHAPE_COMM_COLOR_SHAPE_INFO display_data
	{
		{
			{
				(D_BF_OSD_HISTOGRAM_COMM_BOX_X + D_BF_OSD_HISTOGRAM_COMM_BOX_V_FRAME_THICKNESS),		// SHORT StartX
				(D_BF_OSD_HISTOGRAM_COMM_BOX_Y + D_BF_OSD_HISTOGRAM_COMM_BOX_H_FRAME_THICKNESS)			// SHORT StartY
			}, // T_POS Postion
			(D_BF_OSD_HISTOGRAM_COMM_BOX_WIDTH - (D_BF_OSD_HISTOGRAM_COMM_BOX_V_FRAME_THICKNESS << 1)),	// SHORT Width
			(D_BF_OSD_HISTOGRAM_COMM_BOX_LINES - (D_BF_OSD_HISTOGRAM_COMM_BOX_H_FRAME_THICKNESS << 1))	// SHORT Height
		},
		0
	},
	0//showing_area_No
};

#ifdef CO_BF_OSD_GENERAL_DEBUG_HISTOGRAM
// For debug 0xX0: No show. 0x01 show at OC. 0x11: show at VGR1 0x21: show at VGR0
UINT32 gBF_Osd_Histogram_Comm_Show_Hist_Flg = 0;
#endif
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
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_COMM_CODE"
FJ_ERR_CODE BF_Osd_Histogram_Comm_Set_Histogram(ULONG* histogram_data)
{
	FJ_ERR_CODE ret;
	OS_USER_ER ercd;

	ret  = FJ_ERR_OK;
	ercd = OS_User_Twai_Sem(SID_HISTO_BUF, D_DDIM_SEM_WAIT_FEVR);

	if (ercd != D_OS_USER_E_OK) {
		if (ercd == D_OS_USER_E_TMOUT) {
			BF_Debug_Print_Information(("I:Semaphore Get Time Out for hunt histogram memory. ercd = %d\n", ercd));
			ret = FJ_ERR_NG;
		}
		else {
			BF_Debug_Print_Information(("I:Semaphore Get Error for set histogram. ercd = %d\n", ercd));
			ret = FJ_ERR_NG;
		}
	}
	else {
		if (histogram_data == NULL) {
			// Set histogram data to histogram buffer by Xch. (for PlayBack)
			Im_Xch_Get_Histogram(gBF_Osd_Histogram_Comm_Current_Hist_Buffer);
		}
		else {
			// Set histogram data by argument. (for View)
			memcpy(gBF_Osd_Histogram_Comm_Current_Hist_Buffer, histogram_data,
							(sizeof(ULONG) * D_BF_OSD_HISTOGRAM_COMM_DRAW_BUF_SIZE));
		}

		ercd = OS_User_Sig_Sem(SID_HISTO_BUF);
		if (ercd != D_OS_USER_E_OK) {
			BF_Debug_Print_Information(("I:Semaphore Error : ercd = %d\n", ercd));
			ret = FJ_ERR_NG;
		}
	}
	return ret;
}

VOID BF_Osd_Histogram_Comm_Draw_Histogram_Box(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, 
											  T_BF_OSD_HISTOGRAM_COMM_HIST_ATTR* histogram_size)
{
	// Draw box
	SHORT		temp;
	T_IconColor	Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_END];
	T_Rect		obj_wnd, dispWnd;

	switch( disp_no_data->disp_block_no ) {
		case E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK_HDMI: //HDMI's GR0/1

#if FJ_OSD_COMM_RGBAXXXX == 4
			Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word = gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.hdmi.pen_color.word;
			Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY].word    = gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.hdmi.back_color.word;
#else
			Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.hdmi.pen_color.word);
			Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY].word    = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.hdmi.back_color.word);
#endif
			break;
		default:
			//LCD's GR0/1 

#if FJ_OSD_COMM_RGBAXXXX == 4
			Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word = gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.lcd.pen_color.word;
			Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY].word    = gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.lcd.back_color.word;
#else
			Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_OUTLINE].word = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.lcd.pen_color.word);
			Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY].word    = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.lcd.back_color.word);
#endif
			break;
	}

	histogram_size->display_data.rect_data.Rect.Postion.StartX = histogram_size->box.out_pos_x + histogram_size->box.thickness.StartX;
	histogram_size->display_data.rect_data.Rect.Postion.StartY = histogram_size->box.out_pos_y + histogram_size->box.thickness.StartY;

	histogram_size->draw_data.draw_rect = histogram_size->display_data.rect_data.Rect;
	BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(disp_no_data, &histogram_size->draw_data.draw_rect.Postion);
	
	// Get draw histogram's address
	BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(&histogram_size->draw_data.draw_rect.Postion,
									 &histogram_size->draw_data.write_address, disp_no_data);
	
	obj_wnd.Postion.StartX = histogram_size->box.out_pos_x;
	obj_wnd.Postion.StartY = histogram_size->box.out_pos_y;
	obj_wnd.Width =
	dispWnd.Width = histogram_size->box.out_width;
	obj_wnd.Height =
	dispWnd.Height = histogram_size->box.out_height;
	dispWnd.Postion.StartX =
	dispWnd.Postion.StartY = 0;

	BF_Osd_Shape_Comm_Draw_Shape_GR(disp_no_data, E_FJ_OSD_SHAPE_TYPE_FILLED_RECT_FRAME, obj_wnd, dispWnd,
						 Color_Blending, &histogram_size->box.thickness, E_DrawMode_Speed_Optimized, 255);
	// 1 column can be present unit
	temp = ((histogram_size->box.out_width) - (histogram_size->box.thickness.StartX << 1));
	histogram_size->line.x_offset = D_BF_OSD_HISTOGRAM_COMM_DRAW_BUF_SIZE / temp;
	histogram_size->line.y_offset = histogram_size->threshold / (histogram_size->box.out_height - (histogram_size->box.thickness.StartY << 1));
	histogram_size->display_data.Color_Blending.word = Color_Blending[E_BF_OSD_SHAPE_COMM_SHAPE_PARTS_BODY].word;
	histogram_size->draw_data.draw_rect.Width = 
	histogram_size->display_data.rect_data.Rect.Width = histogram_size->box.out_width - (histogram_size->box.thickness.StartX << 1);
	histogram_size->draw_data.draw_rect.Height = 
	histogram_size->display_data.rect_data.Rect.Height = histogram_size->box.out_height - (histogram_size->box.thickness.StartY << 1);
}

VOID BF_Osd_Histogram_Comm_Draw_Histogram_Graph(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data
												, T_BF_OSD_HISTOGRAM_COMM_HIST_ATTR const* const histogram_size)
{
	UINT32 graph_color;//Histogram's graph color
	UINT32 global_width_display = gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data->disp_block_no]
		[disp_no_data->gr_no].area_size[disp_no_data->draw_area].Width;

	
	switch(disp_no_data->layer_name ) {
		case E_OSDLayer_HDMI_OSD0: //FALL THROUGH
#if FJ_OSD_COMM_RGBAXXXX == 4
			graph_color = gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.hdmi.graph_color.word;
#else
			graph_color = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.hdmi.graph_color.word);
#endif
			break;

		case E_OSDLayer_HDMI_OSD1: //FALL THROUGH
#if FJ_OSD_COMM_RGBAXXXX == 4
			graph_color = gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.hdmi.graph_color.word;
#else
			graph_color = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.hdmi.graph_color.word);
#endif
			break;

		case E_OSDLayer_LCD_OSD0:
			//E_OSDLayer_LCD_OSD0
#if FJ_OSD_COMM_RGBAXXXX == 4
			graph_color = gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.lcd.graph_color.word;
#else
			graph_color = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.lcd.graph_color.word);
#endif
			break;

		default:
			//E_OSDLayer_LCD_OSD1
#if FJ_OSD_COMM_RGBAXXXX == 4
			graph_color = gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.lcd.graph_color.word;
#else
			graph_color = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data->disp_block_no].histogram.lcd.graph_color.word);
#endif
			break;
	}

	if (OS_User_Twai_Sem(SID_HISTO_BUF, D_DDIM_SEM_WAIT_FEVR) == D_OS_USER_E_OK) {
		ULONG current_draw_buffer[D_BF_OSD_HISTOGRAM_COMM_DRAW_BUF_SIZE];	// current histogram average of each X column
		USHORT i, current_draw_count = 0;
		ULONG j = 0;
		USHORT histogram_height = (histogram_size->box.out_height - (histogram_size->box.thickness.StartY << 1));

		// current_his_buffer[max_value_index];
		for (i = 0; i < D_BF_OSD_HISTOGRAM_COMM_DRAW_BUF_SIZE; i += histogram_size->line.x_offset) {
			// Get Histogram' average by X column
			current_draw_buffer[current_draw_count] = 0;
			for (j = 0; j < histogram_size->line.x_offset; j++) {
				current_draw_buffer[current_draw_count] += gBF_Osd_Histogram_Comm_Current_Hist_Buffer[i + j];
			}
			current_draw_buffer[current_draw_count] /= histogram_size->line.x_offset;

			// Get the different from last time
			current_draw_buffer[current_draw_count] = BF_Osd_General_Get_Min((current_draw_buffer[current_draw_count] / histogram_size->line.y_offset), histogram_height);
			current_draw_count++;
		}
		if (OS_User_Sig_Sem(SID_HISTO_BUF) == D_OS_USER_E_OK) {
			BF_Osd_Shape_Comm_Draw_Rect_GR(histogram_size->draw_data.draw_rect, global_width_display,
						histogram_size->display_data.Color_Blending, histogram_size->draw_data.write_address);

			for (i = 0; i < current_draw_count; i++) {
				for (j = 1; j <= current_draw_buffer[i]; j++) {
					// Write Y address
					UINT32 opposit_y = (histogram_height - j);
					*((OSD_USHORT_INT*)(histogram_size->draw_data.write_address) + (opposit_y * global_width_display + i)) = (OSD_USHORT_INT)graph_color;
				}
			}
		}
	}
}

#ifdef CO_BF_OSD_GENERAL_DEBUG_HISTOGRAM
VOID BF_Osd_Histogram_Comm_Show_Histogram(VOID)
{
	if (gBF_Osd_Histogram_Comm_Show_Hist_Flg & 0x01) {
		E_OSDLayer his_layer = (E_OSDLayer)(gBF_Osd_Histogram_Comm_Show_Hist_Flg >> 4);

		FJ_OSD_COMM_FLUSH_DARA_AREA_INFO switch_buffer_infor = {
			.flush_layers = his_layer,
			.flush_showing_area = 0,
			.Is_Copy_flush_showing_area = 0,
		};

		switch (his_layer) {
			case E_OSDLayer_LCD_OSD1:
				switch_buffer_infor.flush_showing_area = 
				switch_buffer_infor.Is_Copy_flush_showing_area = gBF_Osd_Histogram_Comm_LCD_Hist_Attr.showing_area_No+1;
//				FJ_HistogramEnable(his_layer);
//				FJ_OSD_Flush_Draw_Area(&switch_buffer_infor,1);
				break;

			case E_OSDLayer_LCD_OSD0:
				switch_buffer_infor.flush_showing_area = 
				switch_buffer_infor.Is_Copy_flush_showing_area = gBF_Osd_Histogram_Comm_LCD_Hist_Attr.showing_area_No+1;
//				FJ_HistogramEnable(his_layer);
//				FJ_OSD_Flush_Draw_Area(&switch_buffer_infor,1);
				break;

			case E_OSDLayer_HDMI_OSD0:		// FALL THROUGH
				switch_buffer_infor.flush_showing_area = 
				switch_buffer_infor.Is_Copy_flush_showing_area = gBF_Osd_Histogram_Comm_HDMI_Hist_Attr.showing_area_No+1;
//				FJ_HistogramEnable(his_layer);
//				FJ_OSD_Flush_Draw_Area(&switch_buffer_infor,1);
				break;

			case E_OSDLayer_HDMI_OSD1:		// FALL THROUGH
				switch_buffer_infor.flush_showing_area = 
				switch_buffer_infor.Is_Copy_flush_showing_area = gBF_Osd_Histogram_Comm_HDMI_Hist_Attr.showing_area_No+1;
//				FJ_HistogramEnable(his_layer);
//				FJ_OSD_Flush_Draw_Area(&switch_buffer_infor,1);
				break;

			default:
				break;
		}
	}
}
#endif

