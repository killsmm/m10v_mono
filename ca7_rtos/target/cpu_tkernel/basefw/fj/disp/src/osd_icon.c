/**
 * @file		osd_icon.c
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#ifndef INCLUDE_MADE
#include "osd_icon.h"
#include "osd_attr.h"
#include "osd_icon_cache.h"

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

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// CLUT-OS_USER_ID(4bit) is acquired from font data (8bit).
#define osd_icon_cnv_clut_id_half_byte_to_1byte(clutno, width_index)	\
							(((width_index & 1) == 0) ? (clutno >> 4) :	\
							(clutno & 0x0f))

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/

///////////////////////////////////////////////////
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_COMM_CODE"
/**
Get format of ICON by Font-ID.
@param[in]	rgba_icon_bit_num	icon bit number
@return		Format of ICON
*/
static E_BF_OSD_ICON_COMM_ICON_FMT osd_icon_get_icon_fmt(E_BF_OSD_ICON_COMM_ICON_FORMAT rgba_icon_bit_num)
{
	E_BF_OSD_ICON_COMM_ICON_FMT format;

	switch(rgba_icon_bit_num) {
		case E_BF_OSD_ICON_COMM_ICON_FORMAT_16BIT://2Byte date
			format = E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_16bit;
			break;
		case E_BF_OSD_ICON_COMM_ICON_FORMAT_8BIT://1Byte date
			format = E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_8bit;
			break;
		case E_BF_OSD_ICON_COMM_ICON_FORMAT_4BIT://0.5Byte date
			format = E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_4bit;
			break;
		case E_BF_OSD_ICON_COMM_ICON_FORMAT_2BIT://1/4 Byte date
			format = E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_2bit;
			break;
		case E_BF_OSD_ICON_COMM_ICON_FORMAT_1BIT://1/8 Byte date 
			format = E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_1bit;
			break;
		default: //E_BF_OSD_ICON_COMM_ICON_FORMAT_RGBA: //4Byte date
			format = E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON;
			break;
	}

	return format;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
///////////////////////////////////////////////////
//  Following are BASEFW_BOOT_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_BOOT_CODE"


///////////////////////////////////////////////////
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_COMM_CODE"
FJ_ERR_CODE BF_Osd_Icon_Draw_GR_Icon(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, const T_POS* draw_positon, const T_BF_OSD_ICON_COMM_ICON_INFO* src_addr
									,E_DrawMode DrawMode, BYTE alpha)
{
	FJ_ERR_CODE ret = FJ_ERR_OK;
	ULONG gr_Write_infor;
	USHORT screen_size[2];//Screen size.[0]: width [1]:height
	UINT32	buffer_size;

	BF_Osd_Attr_Get_Draw_Area_Size(disp_no_data,  screen_size);
	BF_Osd_Draw_Comm_Get_GR_Draw_Start_Addr(draw_positon, &gr_Write_infor, disp_no_data);

	T_BF_OSD_ICON_COMM_DRAW_ICON_INFO  iconInfo;
	iconInfo.icon_data = *src_addr;
	iconInfo.display_window.Postion = *draw_positon;
	iconInfo.display_window.Width =  src_addr->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH];
	iconInfo.display_window.Height =  src_addr->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES];
	iconInfo.string_iocn_pos = *draw_positon;
	iconInfo.rgba_palette_table = (OSD_USHORT_INT*)BF_Osd_Icon_Get_Palette_Buffer_Addr(gBF_Osd_Icon_Font_ID, &buffer_size, FALSE);

	BF_Osd_Icon_Comm_Recalculate_Icon_Addr(osd_icon_get_icon_fmt((E_BF_OSD_ICON_COMM_ICON_FORMAT)src_addr->icon_format), &iconInfo);

	// Write RGBA data
	switch(DrawMode){
		case E_DrawMode_Speed_Optimized:
			//No any skip color
			BF_Osd_Icon_Comm_Copy_Box_To_Box(&iconInfo, screen_size[D_BF_OSD_ATTR_INDEX_WIDTH], gr_Write_infor);
			break;
		default:
			BF_Osd_Icon_Comm_Draw_Icon_By_Bending(disp_no_data->disp_block_no, screen_size[D_BF_OSD_ATTR_INDEX_WIDTH],
												&iconInfo,alpha,gr_Write_infor);
			break;
	}

	return ret;
}


#endif //INCLUDE_MADE
