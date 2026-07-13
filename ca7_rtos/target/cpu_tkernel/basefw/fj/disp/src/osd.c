/**
 * @file		osd.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
// This file will be included to fj_osd.c. The section define is belong to that file
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#ifndef INCLUDE_MADE
#include "fj_disp.h"
#include "osd_attr.h"
#include "osd_config.h"
#include "osd_draw.h"
#include "osd_histogram_comm.h"
#include "osd_icon.h"
#include "osd_image.h"
#include "osd_shape.h"
#include "memory.h"
#include "play_common.h"

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
// Nothing Special

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
///////////////////////////////////////////////////
//  Following are BASEFW_BOOT_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_BOOT_CODE"
/**
Configures the OSD display attributes.
@param[in] disp_type	display type.<br>
			@ref FJ_DISP_TYPE_LCD		: LCD<br>
			@ref FJ_DISP_TYPE_HDMI		: HDMI<br>
@param[in] layer_id layer id.<br>
			id is valid when Attr is as following:<br>
			@ref E_FJ_OSD_ATTR_HISTO_PEN_COLOR<br>
			@ref E_FJ_OSD_ATTR_HISTO_GRAF_COLOR<br>
			@ref E_FJ_OSD_ATTR_HISTO_GRAF_BACK_COLOR<br>
@param[in] Attr attribute type.<br>
			@ref E_FJ_OSD_ATTR_CENTER : Set OSD display left-top's coordinate.<br>
				&nbsp;&nbsp;&nbsp; wParam1 - x coordinate<br>
				&nbsp;&nbsp;&nbsp; wParam2 - y coordinate<br>
			@ref E_FJ_OSD_ATTR_BRUSH_COLOR : Set shape's body color.<br>
				&nbsp;&nbsp;&nbsp; wParam1 - Please specify RGBA color when drawing to OSD0/OSD1/HDMI_OSD0/HDMI_OSD1.<br>
			@ref E_FJ_OSD_ATTR_PEN_COLOR - Set shape's frame color<br>
				&nbsp;&nbsp;&nbsp; wParam1 - Please specify RGBA color when drawing to OSD0/OSD1/HDMI_OSD0/HDMI_OSD1.<br>
			@ref E_FJ_OSD_ATTR_TEXT_SPACE : Sets text spaces<br>
				&nbsp;&nbsp;&nbsp; wParam1 - Width space between OSD icons in one string.<br>
			@ref E_FJ_OSD_ATTR_FRAME_THICKNESS - Configures the thickness of the frames<br> 
			that are drawn by FJ_OSDDrawShape API.(negative thickness indicates external frame).<br>
				&nbsp;&nbsp;&nbsp; wParam1 - X thickness in pixels.<br>
				&nbsp;&nbsp;&nbsp; wParam2 - Y thickness in pixels.<br>
			@ref E_FJ_OSD_ATTR_HISTO_PEN_COLOR : Set histogram's frame color<br>
			@ref E_FJ_OSD_ATTR_HISTO_GRAF_COLOR : Set histogram's color<br>
			@ref E_FJ_OSD_ATTR_HISTO_GRAF_BACK_COLOR : Set histogram's background color<br>
			When this setting is done, it is necessary to specify the layer with layer_id.<br>
				&nbsp;&nbsp;&nbsp; wParam1 - Please specify RGBA color when drawing to OSD0/OSD1/HDMI_OSD0/HDMI_OSD1.<br>
			@ref E_FJ_OSD_ATTR_REPLACE_COLOR : Replace a color to other one.<br>
			When drawing an OSD/HDMI OSD icon, a old color(wParam1) is replaced to a new color(wParam2).<br>
				&nbsp;&nbsp;&nbsp; wParam1: Old color by RGBA<br>
				&nbsp;&nbsp;&nbsp; wParam2: Replace color by RGBA<br>
			@ref E_FJ_OSD_ATTR_ROUND_RECT_RADIUS : Round rectangle's corner's radius<br>
				&nbsp;&nbsp;&nbsp; wParam1 - Round rectangle's corner's radius.<br>
			@ref E_FJ_OSD_ATTR_DONUT_FRAME_SIZE : Specify the donut rectangle's frame size.<br>
				&nbsp;&nbsp;&nbsp; wParam1 - The donut rectangle's frame size.<br>
			@ref E_FJ_OSD_ATTR_TEXT_SIZE : Sets text size:<br>
				&nbsp;&nbsp;&nbsp; wParam1 - The horizontal size of character.<br>
				&nbsp;&nbsp;&nbsp; wParam2 - The vertical size of character.<br>
			@ref E_FJ_OSD_ATTR_IMG_BACK_COLOR : Background color at the time of drawing image to image layer.<br>
				&nbsp;&nbsp;&nbsp; wParam1 - Background color drawing enable.(1:Enable, 0:Disable)<br>
				&nbsp;&nbsp;&nbsp; wParam2 - Background color.(YCbCr format)<br>
@param[in] wParam1 parmater 1.<br>
@param[in] wParam2 parmater 2.<br>
@param[in] wParam3 parmater 3.<br>
@param[in] wParam4 parmater 4.<br>
@retval FJ_ERR_OK	Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
@retval FJ_ERR_OSD_LAYER	Abnormal End. Layer error.
@retval FJ_ERR_NG	Abnormal End.
*/
static FJ_ERR_CODE fj_setosddisplayattr(FJ_DISP_TYPE disp_type, 
										E_OSDLayer layer_id, E_FJ_OSD_ATTR_TYPE Attr,
										INT32 wParam1, INT32 wParam2, INT32 wParam3, INT32 wParam4)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	E_IM_DISP_SEL block = E_IM_DISP_HDMI;

	if( disp_type == FJ_DISP_TYPE_HDMI ){
		block = E_IM_DISP_HDMI;
	}
	else if( disp_type == FJ_DISP_TYPE_LCD ){
		block = E_IM_DISP_LCD_MIPI;
	}
	else{
		ret = FJ_ERR_NG;
	}

	switch(Attr) {
		case E_FJ_OSD_ATTR_TEXT_CLUT:		// FALL THROUGH
		case E_FJ_OSD_ATTR_BITMAP_COLOR:
			// Parameter error.
			ret = FJ_ERR_NG_INPUT_PARAM;
			break;

		case E_FJ_OSD_ATTR_TEXT_SIZE:
			// Set text size.
			gBF_Osd_Draw_Comm_Osd_Attr[block].text.text_size.StartX = wParam1;	// Width
			gBF_Osd_Draw_Comm_Osd_Attr[block].text.text_size.StartY = wParam2;	// Height
			break;

		case E_FJ_OSD_ATTR_IMG_BACK_COLOR:
			// Set background color for drawing image to Vch/HDch.
			if (wParam1) {
				// Enable background color drawing.
				gBF_Osd_Draw_Comm_Osd_Attr[block].color.img_back.enable = TRUE;
			}
			else {
				// Disable background color drawing.
				gBF_Osd_Draw_Comm_Osd_Attr[block].color.img_back.enable = FALSE;
			}
			gBF_Osd_Draw_Comm_Osd_Attr[block].color.img_back.color.word = wParam2; // Color
			break;

		default:
			// Call common setting processing in "Made" and "Non-Made". 
			ret = BF_Osd_Draw_Comm_Set_Comm_Display_Attr(layer_id, Attr, wParam1, wParam2, wParam3, wParam4);
			break;
	}
	return ret;
}


/**
Draw an icon string.
@param[in] layer_id layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0	: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1	: LCD OSD-1 layer<br>
			@ref E_OSDLayer_LCD_GRID	: LCD Grid line layer<br>
			@ref E_OSDLayer_LCD_FACE	: LCD Face frame layer<br>
			@ref E_OSDLayer_HDMI_OSD0	: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1	: HDMI OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_GRID	: HDMI Grid line layer<br>
			@ref E_OSDLayer_HDMI_FACE	: HDMI Face frame layer<br>
@param[in] wStartX X of starting position to draw.
@param[in] wStartY Y of starting position to draw.
@param[in] wLength output icon length in the string.<br>"wLength=0" means full length of string.<br>
@param[in] uwStringID string ID.
@param[in] DrawMode draw mode. See @ref E_DrawMode.<br>
			@ref E_DrawMode_Speed_Optimized : Write drawing data directly.<br>
			@ref E_DrawMode_Effect_Optimized : Blend drawing data with the color of the place drawn to. 
@param[in]  alpha attribute level for blending when a icon is drawn. It's valid when DrawMode is @ref E_DrawMode_Effect_Optimized only.<br>
            And it's valid on GR layer only. It can be set 0~255. 
			e.g.) It's set to 255 if the writting data 100% will be adopted.<br>
			      There are not any changed if it's set 0.<br>
				  The writting icon will be  blended with destination color by 50%-50% if it is set to 128.
@param[in]	drawAreaNum	number of Area[0-9].<br>
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
@retval FJ_ERR_OSD_LAYER		Abnormal End. layer_id is illegal.
@retval FJ_ERR_OSD_SEQUENCE		Abnormal End. Icon, string or CLUT data has not been load.
@retval FJ_ERR_POSITION			Abnormal End. The drawing position is illegal. 
@retval FJ_ERR_NG				Abnormal End.
@remarks Draw a string of icon.<br>
		 The @ref FJ_DirectSelectFont should be called to set icon kind before this API is called.
*/
static FJ_ERR_CODE fj_osdtextoutid(E_OSDLayer layer_id, INT32 wStartX, INT32 wStartY, INT32 wLength, INT32 uwStringID
								,E_DrawMode DrawMode, BYTE alpha, BYTE drawAreaNum)
{
	INT32 i;

	// Check resource load status
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	T_POS draw_position = {
		(SHORT)wStartX,(SHORT)wStartY
	};

	FJ_ERR_CODE ret = BF_Osd_Draw_Comm_Get_Block_GR(layer_id, &disp_no_data);
	if (ret == FJ_ERR_OK) {
		ret = drawAreaNum < gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data.disp_block_no][disp_no_data.gr_no].vaild_area_num ? FJ_ERR_OK : FJ_ERR_NG_INPUT_PARAM;
	}

	if (ret == FJ_ERR_OK) {
		disp_no_data.draw_area = drawAreaNum;
		if(BF_Osd_Icon_Is_Icon_Loaded(gBF_Osd_Icon_Font_ID) && BF_Osd_Icon_Is_String_Loaded(gBF_Osd_Icon_Font_ID)) {
			if(uwStringID < *gBF_Osd_Icon_String_Table_Num[gBF_Osd_Icon_Font_ID]) {
				T_BF_OSD_ICON_COMM_DRAW_ICON_INFO iconInfo[MAX_ICON_NUM_IN_1STRING]; //every iocn's position by alligement
				INT32 icon_num;
				USHORT string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_END] = {0, 0}; //string draw rectangle

				icon_num = BF_Osd_Icon_Get_String_Draw_Range(disp_no_data.disp_block_no, uwStringID, gBF_Osd_Icon_Font_ID, iconInfo, string_size);

				ret = BF_Osd_Draw_Comm_Is_Rect_Draw_Area_Inside(&disp_no_data, draw_position, string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH], string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES]);
				if(ret == FJ_ERR_OK){
					T_POS	*text_size, *space_size;
					text_size = &(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data.disp_block_no].text.text_size);
					space_size = &(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data.disp_block_no].text.space_size);

					//valid string OS_USER_ID
					//Calculate codinate
					BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(&disp_no_data, &draw_position);
	
					if((USHORT)wLength == 0) {
						wLength = icon_num;
					}
					else{
						wLength = BF_Osd_General_Get_Min((USHORT)wLength, icon_num);
					}
	
					for(i = 0; i < wLength; i++) {
						ret = BF_Osd_Icon_Draw_GR_Icon(&disp_no_data, &draw_position, &iconInfo[i].icon_data, DrawMode, alpha);
						if(ret != FJ_ERR_OK) {
							break;
						}
						draw_position.StartX += (((text_size->StartX > 0) && (text_size->StartY > 0)) ?
								(space_size->StartX + text_size->StartX) :
								(space_size->StartX + iconInfo[i].icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH]));
					}
				}
				else {
					ret = FJ_ERR_NG_INPUT_PARAM;
				}
			}
			else {
				ret = FJ_ERR_NG_INPUT_PARAM;
			}
		}
		else {
			// Resource data is not loaded
			ret = FJ_ERR_OSD_SEQUENCE;
		}
	}
	return ret;
}

/**
Display image file.<br>
@param[in] layer_id layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0	: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1	: LCD OSD-1 layer<br>
			@ref E_OSDLayer_LCD_GRID	: LCD Grid line layer<br>
			@ref E_OSDLayer_LCD_FACE	: LCD Face frame layer<br>
			@ref E_OSDLayer_HDMI_OSD0	: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1	: HDMI OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_GRID	: HDMI Grid line layer<br>
			@ref E_OSDLayer_HDMI_FACE	: HDMI Face frame layer<br>
@param[in] wFileType type of image file.<br>
			@ref FJ_GR_FILE_JPEG - Jpeg file<br>
			@ref FJ_GR_FILE_JPEG_THUMBNAIL - Jpeg file's thumbnail<br>
			@ref FJ_GR_FILE_SCREENNAIL - Jpeg file's Screennail<br>
			@ref FJ_GR_FILE_JPEG_QVGA - Same as @ref FJ_GR_FILE_SCREENNAIL<br>
			@ref FJ_GR_FILE_PHOTOFRAME - PhotoFrame<br>
			@ref FJ_GR_FILE_RGBA8888 - RGBA8888 format file<br>
@param[in] FileName image file name.<br>
@param[in] uStartX X of starting position to draw.<br>
@param[in] uStartY Y of starting position to draw.<br>
@param[in] uWidth width of the shape.<br>
@param[in] uHight height of the shape.<br>
@param[in] drawAreaNum	number of Area[0-9].<br>
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG_INPUT_PARAM	Abnormal End. Input parameter error.
@retval FJ_ERR_OSD_LAYER		Abnormal End. layerID is illegal.
@retval FJ_ERR_NG				Abnormal End.
@remarks The combination of DisplayID and wFileType that can be specified is as follows.<br>
		 <table>
		   <tr>
		     <th colspan="2" rowspan="2">&nbsp;</th>
			 <th colspan="7">DisplayID</th>
		   </tr>
		   <tr>
			 <td>@ref E_OSDLayer_LCD_MAIN</td>
			 <td>@ref E_OSDLayer_HDMI_MAIN</td>
			 <td>@ref E_OSDLayer_LCD_OSD0</td>
			 <td>@ref E_OSDLayer_LCD_OSD1</td>
			 <td>@ref E_OSDLayer_HDMI_OSD0</td>
		   </tr>
		   <tr>
		     <th rowspan="8">wFileType</td>
			 <td>@ref FJ_GR_FILE_JPEG</td>
			 <td align="center">o</td>
			 <td align="center">o</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
		   </tr>
		   <tr>
			 <td>@ref FJ_GR_FILE_JPEG_THUMBNAIL</td>
			 <td align="center">o</td>
			 <td align="center">o</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
		   </tr>
		   <tr>
			 <td>@ref FJ_GR_FILE_SCREENNAIL</td>
			 <td align="center">o</td>
			 <td align="center">o</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
		   </tr>
		   <tr>
			 <td>@ref FJ_GR_FILE_JPEG_QVGA</td>
			 <td align="center">o</td>
			 <td align="center">o</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
		   </tr>
		   <tr>
			 <td>@ref FJ_GR_FILE_PHOTOFRAME</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
			 <td align="center">o</td>
			 <td align="center">o</td>
			 <td align="center">o</td>
		   </tr>
		   <tr>
			 <td>@ref FJ_GR_FILE_COMPRESS</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
			 <td align="center">o</td>
			 <td align="center">o</td>
			 <td align="center">o</td>
		   </tr>
		   <tr>
			 <td>@ref FJ_GR_FILE_RGBA8888</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
			 <td align="center">o</td>
			 <td align="center">o</td>
			 <td align="center">o</td>
		   </tr>
		   <tr>
			 <td>@ref FJ_GR_FILE_ICON</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
			 <td align="center">x</td>
		   </tr>
		 </table>
*/
FJ_ERR_CODE fj_drawfile(E_FJ_DISP_COMM_BLOCK block,
							E_OSDLayer layerID, FJ_GR_FILE_TYPE wFileType, CHAR* FileName,
							   USHORT uStartX, USHORT uStartY, USHORT uWidth, USHORT uHight, BYTE drawAreaNum)
{
	FJ_ERR_CODE	ret = FJ_ERR_OK;

	uStartX += gBF_Osd_Draw_Comm_Osd_Attr[block].common.left_top_pos.StartX;
	uStartY += gBF_Osd_Draw_Comm_Osd_Attr[block].common.left_top_pos.StartY;

	switch (layerID) {
		case E_OSDLayer_LCD_MAIN: 
			switch (wFileType) {
				case FJ_GR_FILE_JPEG:           // JPEG Main  (FALL THROUGH)
				case FJ_GR_FILE_JPEG_THUMBNAIL: // Thumbnail  (FALL THROUGH)
				case FJ_GR_FILE_JPEG_QVGA:      // Screennail (FALL THROUGH)
				case FJ_GR_FILE_SCREENNAIL: {    // Screennail
					// Draw JPEG to Vch.
					T_BF_PLAY_COMMON_DRAW_PARAM	draw_param;
					T_BF_PLAY_MACRO_WRAP_JPEG_DEC_MNG	jpgmng;
					U_IM_DISP_SIZE	imgsiz;

					memset(&jpgmng, 0, sizeof(jpgmng));

					// The size of LVch is checked when outputting to LCD.
					Im_DISP_Get_Input_Size(E_IM_DISP_LCD_MIPI, &imgsiz);
					// Set destination area info
					draw_param.gl_width	= imgsiz.size.width;
					draw_param.gl_lines	= imgsiz.size.lines;
					draw_param.width	= uWidth;
					draw_param.lines	= uHight;
					draw_param.pos_x	= uStartX;
					draw_param.pos_y	= uStartY;
					draw_param.file_name = FileName;
					draw_param.bg_color.is_darw  = gBF_Osd_Draw_Comm_Osd_Attr[block].color.img_back.enable;
					draw_param.bg_color.y_color  = gBF_Osd_Draw_Comm_Osd_Attr[block].color.img_back.color.AYCC.Y;
					draw_param.bg_color.cb_color = gBF_Osd_Draw_Comm_Osd_Attr[block].color.img_back.color.AYCC.Cb;
					draw_param.bg_color.cr_color = gBF_Osd_Draw_Comm_Osd_Attr[block].color.img_back.color.AYCC.Cr;
					Sdram_Map_Osd_Get_VDE_Chg_Addr(FJ_DISP_TYPE_LCD, &draw_param.addr_y, &draw_param.addr_c);
					// Draw YCbCr data
					ret = BF_Play_Common_Draw_Jpeg(wFileType, &draw_param, E_BF_PLAY_COMMON_EXEC_MODE_DEC_DISP, &jpgmng);
					}
					break;
				default:
					ret = FJ_ERR_NG_INPUT_PARAM;
					break;
			}
			break;

		case E_OSDLayer_HDMI_MAIN:
			switch (wFileType) {
				case FJ_GR_FILE_JPEG:           // JPEG Main  (FALL THROUGH)
				case FJ_GR_FILE_JPEG_THUMBNAIL: // Thumbnail  (FALL THROUGH)
				case FJ_GR_FILE_JPEG_QVGA:      // Screennail (FALL THROUGH)
				case FJ_GR_FILE_SCREENNAIL:     // Screennail
					// Show to HD
					ret = BF_Osd_Image_Comm_Draw_Jpeg_To_HD( wFileType, FileName, uStartX, uStartY, uWidth, uHight);
					break;
				default:
					ret = FJ_ERR_NG_INPUT_PARAM;
					break;
			}
			break;

		case E_OSDLayer_LCD_OSD1: //(FALL THROUGH)
		case E_OSDLayer_LCD_OSD0: //(FALL THROUGH)
		case E_OSDLayer_HDMI_OSD1: //(FALL THROUGH)
		case E_OSDLayer_HDMI_OSD0:
			ret = BF_Osd_Drawfile(layerID, wFileType, FileName, uStartX, uStartY, uWidth, uHight, drawAreaNum);
			break;

		default:
			ret = FJ_ERR_OSD_LAYER;
			break;
	}
	return ret;
}


///////////////////////////////////////////////////
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_COMM_CODE"

/**
Enables the host to display a given string at a specific location on the display.
@param[in] layerID layer ID.<br>
			@ref E_OSDLayer_LCD_OSD0	: LCD OSD-0 layer<br>
			@ref E_OSDLayer_LCD_OSD1	: LCD OSD-1 layer<br>
			@ref E_OSDLayer_HDMI_OSD0	: HDMI OSD-0 layer<br>
			@ref E_OSDLayer_HDMI_OSD1	: HDMI OSD-1 layer<br>
@param[in] textOutInfo output text information. See @ref T_OSD_TEXT_OUT_BY_ASCII_TABLE_INFO.
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_OSD_LAYER		Abnormal End. DisplayID is illegal.
@retval FJ_ERR_OSD_SEQUENCE		Abnormal End. ICON or CLUT data has not been load.
@retval FJ_ERR_OSD_ASCII_DATA	Abnormal End. ASCII code that was not able to be used was specified. 
@retval FJ_ERR_NG				Abnormal End.
*/
FJ_ERR_CODE fj_osdtextoutbyasciitable(E_OSDLayer layerID, const T_OSD_TEXT_OUT_BY_ASCII_TABLE_INFO* textOutInfo)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	T_BF_SDRAM_MAP_COMMON_OSD disp_no_data;
	USHORT strlength = 0;
	USHORT string_id[MAX_ICON_NUM_IN_1STRING] = {0};
	USHORT string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_END] = {0, 0};
	SHORT temp;
	T_POS draw_position = { (SHORT)textOutInfo->obj_wnd.Postion.StartX,
							(SHORT)textOutInfo->obj_wnd.Postion.StartY };
	
	ret = BF_Osd_Draw_Comm_Get_Block_GR(layerID, &disp_no_data);
	
	if(ret == FJ_ERR_OK){
		if(BF_Osd_Icon_Is_Icon_Loaded(gBF_Osd_Icon_Font_ID) == FALSE) {
			ret = FJ_ERR_OSD_SEQUENCE;
		}
	}
	
	if(ret == FJ_ERR_OK){
		memset(&string_id, 0, sizeof(string_id));
		for (strlength = 0; strlength < MAX_ICON_NUM_IN_1STRING; strlength++) {
			if (textOutInfo->prt_str[strlength] == 0) {
				break;
			}
			else if (textOutInfo->prt_str[strlength] < textOutInfo->tblSize) {
				string_id[strlength] = textOutInfo->asciiTbl[textOutInfo->prt_str[strlength]];
			}
			else {
				ret = FJ_ERR_OSD_ASCII_DATA;
				break;
			}
		}
	}
		
	T_BF_OSD_ICON_COMM_DRAW_ICON_INFO iconInfo[MAX_ICON_NUM_IN_1STRING]; //every iocn's position by alligement
	INT32 icon_num = 0;
	if(ret == FJ_ERR_OK){
		icon_num = BF_Osd_Icon_Calculate_String_Size(disp_no_data.disp_block_no, gBF_Osd_Icon_Font_ID, string_id,
						BF_Osd_General_Get_Min(strlength, MAX_ICON_NUM_IN_1STRING), string_size, iconInfo);
	}
	
	if(icon_num > 0) {
		/* Coordinates where it draws are calculated.
			- In FJ_LEFT case, no need to change X coordinates.
			- In FJ_TOP case, no need to change Y coordinates. */
		if((textOutInfo->formatFlag & FJ_RIGHT) != 0) {
			// Right adjust.
			temp = draw_position.StartX + (textOutInfo->obj_wnd.Width - (SHORT)string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH]);
			draw_position.StartX = BF_Osd_General_Get_Max(draw_position.StartX, temp);
		}
		else if((textOutInfo->formatFlag & FJ_CENTER) != 0) {
			// Horizontal center adjust.
			temp = (SHORT)draw_position.StartX + ((textOutInfo->obj_wnd.Width - (SHORT)string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH]) >> 1);
			draw_position.StartX = BF_Osd_General_Get_Max(temp, draw_position.StartX);
		}
		else {
			// DO NOTHING
		}

		if((textOutInfo->formatFlag & FJ_BOTTOM) != 0) {
			// Bottom adjust.
			temp = draw_position.StartY + ((textOutInfo->obj_wnd.Height - (SHORT)string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES]));
			draw_position.StartY = BF_Osd_General_Get_Max(draw_position.StartY, temp);
		}
		else if((textOutInfo->formatFlag & FJ_VCENTER) != 0) {
			// Vertical center adjust.
			temp = draw_position.StartY + ((textOutInfo->obj_wnd.Height - (SHORT)string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES]) >> 1);
			draw_position.StartY = BF_Osd_General_Get_Max(draw_position.StartY, temp);
		}
		else {
			// DO NOTHING
		}
	}
	else{
		ret = FJ_ERR_OSD_ASCII_DATA;
	}

	disp_no_data.draw_area = textOutInfo->drawAreaNum;
	if(ret == FJ_ERR_OK){
		ret = BF_Osd_Draw_Comm_Is_Rect_Draw_Area_Inside(&disp_no_data, draw_position,
														string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH],
														string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES]);
	}
	
	if(ret == FJ_ERR_OK){
		INT32 i;
		T_POS	*text_size, *space_size;
		text_size = &(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data.disp_block_no].text.text_size);
		space_size = &(gBF_Osd_Draw_Comm_Osd_Attr[disp_no_data.disp_block_no].text.space_size);

		//valid string OS_USER_ID
		//Calculate codinate
		BF_Osd_Draw_Comm_Convert_To_Darw_Area_Pos(&disp_no_data, &draw_position);
	
		for(i = 0; i < icon_num; i++) {
			ret = BF_Osd_Icon_Draw_GR_Icon(&disp_no_data, &draw_position, &iconInfo[i].icon_data, 
										   textOutInfo->DrawMode, textOutInfo->alpha);
			if(ret != FJ_ERR_OK) {
				break;
			}

			draw_position.StartX += (((text_size->StartX > 0) && (text_size->StartY > 0)) ?
					(space_size->StartX + text_size->StartX) :
					(space_size->StartX + iconInfo[i].icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH]));
		}
	}

	return ret;
}
/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
// Nothing Special

#endif //INCLUDE_MADE
