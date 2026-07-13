/**
 * @file		osd_attr.c
 * @brief		
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
//#pragma arm section rwdata="BASEFW_BOOT_INIT"
//#pragma arm section rodata="BASEFW_BOOT_CONST"
//#pragma arm section code="BASEFW_BOOT_CODE"

#include "osd_attr.h"
#include "mba_if.h"

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
/** View freeze flag */
static E_OSD_VIEW_FREEZE gBF_Osd_Attr_View_Freeze_Flg = E_OSD_VIEW_FREEZE_OFF;

#ifdef DEBUG_ON_PC
// to avoid segmentation fault
UCHAR g_pc_debug_main_y[0x300000];
UCHAR g_pc_debug_main_c[0x300000];
UCHAR g_pc_debug_osd[0x300000];
UCHAR g_pc_debug_work[0x300000];
UCHAR g_pc_debug_icon[0xA00000];
UCHAR g_pc_debug_string[0x1000];
UCHAR g_pc_debug_palette[0x1000];
#endif // DEBUG_ON_PC

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
E_OSD_VIEW_FREEZE BF_Osd_Attr_Get_View_Freeze(VOID)
{
	return gBF_Osd_Attr_View_Freeze_Flg;
}

VOID BF_Osd_Attr_Set_View_Freeze(E_OSD_VIEW_FREEZE freeze)
{
	gBF_Osd_Attr_View_Freeze_Flg = freeze;
}

///////////////////////////////////////////////////
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_COMM_CODE"
FJ_ERR_CODE BF_Osd_Attr_Set_Display_Interface(FJ_DISP_TYPE disp_type, E_FJ_DISP_IFS interface)
{
	UINT32 hdmi_layer = E_IM_DISP_SEL_LAYER_NONE;
	UINT32 lcd_layer = E_IM_DISP_SEL_LAYER_NONE;
	BYTE ifs = D_IM_DISP_IFS_NONE;
	E_IM_DISP_SEL disp_sel = E_IM_DISP_LCD_MIPI;
	INT32 ret = D_DDIM_OK;

	switch(disp_type){
		case FJ_DISP_TYPE_LCD:
			disp_sel = E_IM_DISP_LCD_MIPI;
			break;

		case FJ_DISP_TYPE_HDMI:
			disp_sel = E_IM_DISP_HDMI;
			break;

		default:
			BF_Debug_Print_Warning(( "BF_Osd_Attr_Set_Display_Interface(): disp_type is illegal value.\n" ));
			return FJ_ERR_NG;
			break;
	}

	switch(interface){
		case E_FJ_DISP_IFS_NONE:
			ifs = D_IM_DISP_IFS_NONE;
			break;

		case E_FJ_DISP_IFS_LCD:
			ifs = D_IM_DISP_IFS_LCD;
			break;

		case E_FJ_DISP_IFS_HDMI:
			ifs = D_IM_DISP_IFS_HDMI;
			break;

		case E_FJ_DISP_IFS_MIPI:
			ifs = D_IM_DISP_IFS_MIPI;
			break;

		default:
			BF_Debug_Print_Warning(( "BF_Osd_Attr_Set_Display_Interface(): interface is illegal value.\n" ));
			return FJ_ERR_NG;
	}

	if(gBF_Osd_Draw_Comm_Osd_Attr[disp_sel].disp.interface == interface){
		BF_Debug_Print_Warning(( "BF_Osd_Attr_Set_Display_Interface(): interface is already set.\n" ));
		return FJ_ERR_OK;
	}

	ret = Im_DISP_Get_SR(&hdmi_layer, &lcd_layer);
	if(ret != D_DDIM_OK){
		BF_Debug_Print_Error(( "BF_Osd_Attr_Set_Display_Interface(): Im_DISP_Get_SR NG. ret=%x\n", ret ));
		return FJ_ERR_NG;
	}

	if( disp_type == FJ_DISP_TYPE_LCD ){
		if( (lcd_layer  & E_IM_DISP_SEL_LAYER_DCORE) != E_IM_DISP_SEL_LAYER_DCORE ) {
			BF_Debug_Print_Error(( "BF_Osd_Attr_Set_Display_Interface(): DCORE is not reset.\n" ));
			return FJ_ERR_NG;
		}
	}
	else{
		if( (hdmi_layer & E_IM_DISP_SEL_LAYER_DCORE) != E_IM_DISP_SEL_LAYER_DCORE ) {
			BF_Debug_Print_Error(( "BF_Osd_Attr_Set_Display_Interface(): DCORE is not reset.\n" ));
			return FJ_ERR_NG;
		}
	}

	ret = Im_DISP_Set_Display_Interface(disp_sel, ifs);
	if(ret != D_DDIM_OK){
		BF_Debug_Print_Error(( "BF_Osd_Attr_Set_Display_Interface(): Im_DISP_Set_Display_Interface NG. ret=%x\n", ret ));
		return FJ_ERR_NG;
	}

	gBF_Osd_Draw_Comm_Osd_Attr[disp_sel].disp.interface = interface;
	M_MBA_PRINTF(( D_MBA_KIND_PREVIEW, FJ_MBALOG_FUNC_ID_LCD, FJ_MBALOG_TYPE_POINT, "IFS=%d", ifs ));
	return FJ_ERR_OK;
}

E_FJ_DISP_IFS BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE disp_type)
{
#if 1
	E_IM_DISP_SEL disp_sel = E_IM_DISP_LCD_MIPI;

	if( disp_type == FJ_DISP_TYPE_LCD ){
		disp_sel = E_IM_DISP_LCD_MIPI;
	}
	else{
		disp_sel = E_IM_DISP_HDMI;
	}
	return gBF_Osd_Draw_Comm_Osd_Attr[disp_sel].disp.interface;
#else
	BYTE ifs;
	E_FJ_DISP_IFS interface = E_FJ_DISP_IFS_NONE;

	// Get display interface selection
	if(Im_DISP_Get_Display_Interface(&ifs) == D_DDIM_OK){
		switch(ifs){
			case D_IM_DISP_IFS_LCD:
				interface = E_FJ_DISP_IFS_LCD;
				break;

			case D_IM_DISP_IFS_HDMI:
				interface = E_FJ_DISP_IFS_HDMI;
				break;

			case D_IM_DISP_IFS_MIPI:
				interface = E_FJ_DISP_IFS_MIPI;
				break;

			default:
				break;
		}
	}

	return interface;
#endif
}

FJ_ERR_CODE BF_Osd_Attr_Get_HDch_Input_Size(T_IMAGE_SIZE* input_size)
{
	FJ_ERR_CODE result = FJ_ERR_OK;
	U_IM_DISP_SIZE hdm_input_size;

	if (BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE_HDMI) == E_FJ_DISP_IFS_HDMI) {
		if (Im_DISP_Get_Input_Size(E_IM_DISP_HDMI, &hdm_input_size) == D_DDIM_OK) {
			input_size->width = hdm_input_size.size.width;
			input_size->lines = hdm_input_size.size.lines;
		}
		else {
			// HIF macro error
			result = FJ_ERR_NG;
		}
	}
	else {
		// Sequence error.(Must be connect HDMI)
		result = FJ_ERR_OSD_SEQUENCE;
	}
	return result;
}

//#pragma arm section code="BASEFW_COMM_CODE"
BYTE BF_Osd_Attr_Get_HDMI_Scan_Mode()
{
	return gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.scan;
}

VOID BF_Osd_Attr_Get_Screen_Size(E_OSDLayer bdisplayid, USHORT screen_size[2])
{
	switch(bdisplayid){
		case E_OSDLayer_LCD_OSD0:
			screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] = D_BF_OSD_GENERAL_OSD_WIDTH;
			screen_size[D_BF_OSD_ATTR_INDEX_LINES] = D_BF_OSD_GENERAL_OSD_LINES;
			break;

		case E_OSDLayer_LCD_OSD1:
			screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] = D_BF_OSD_GENERAL_OSD_WIDTH;
			screen_size[D_BF_OSD_ATTR_INDEX_LINES] = D_BF_OSD_GENERAL_OSD_LINES;
			break;

		case E_OSDLayer_HDMI_OSD0:
			screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] = gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.osd[E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0].width;
			screen_size[D_BF_OSD_ATTR_INDEX_LINES] = gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.osd[E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_0].lines;
			break;

		case E_OSDLayer_HDMI_OSD1:
			screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] = gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.osd[E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1].width;
			screen_size[D_BF_OSD_ATTR_INDEX_LINES] = gBF_Osd_Draw_Comm_Osd_Attr[E_IM_DISP_HDMI].hdmi.osd[E_BF_SDRAM_MAP_COMMON_OSD_GR_NO_1].lines;
			break;

		default:
			break;
	}
}

VOID BF_Osd_Attr_Get_Draw_Area_Size(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, USHORT screen_size[2])
{
	T_Rect* area_size = &gBF_Osd_Draw_Comm_Showing_Area_Inf[disp_no_data->disp_block_no][disp_no_data->gr_no].area_size[disp_no_data->draw_area];

	screen_size[D_BF_OSD_ATTR_INDEX_WIDTH] = area_size->Width;
	screen_size[D_BF_OSD_ATTR_INDEX_LINES] = area_size->Height;
}

#ifdef DEBUG_ON_PC
VOID BF_Osd_Attr_Get_Pc_Debug_Main_Addr(ULONG* y_addr, ULONG* c_addr)
{
	if(y_addr) *y_addr = (ULONG)g_pc_debug_main_y;
	if(c_addr) *c_addr = (ULONG)g_pc_debug_main_c;
}

ULONG BF_Osd_Attr_Get_Pc_Debug_Osd_Addr(ULONG* addr)
{
	if(addr) *addr = (ULONG)g_pc_debug_osd;
	return sizeof(g_pc_debug_osd);
}

ULONG BF_Osd_Attr_Get_Pc_Debug_Work_Addr(ULONG* addr)
{
	if(addr) *addr = (ULONG)g_pc_debug_work;
	return sizeof(g_pc_debug_work);
}

ULONG BF_Osd_Attr_Get_Pc_Debug_Icon_Addr(ULONG* addr)
{
	if(addr) *addr = (ULONG)g_pc_debug_icon;
	return sizeof(g_pc_debug_icon);
}

ULONG BF_Osd_Attr_Get_Pc_Debug_String_Addr(ULONG* addr)
{
	if(addr) *addr = (ULONG)g_pc_debug_string;
	return sizeof(g_pc_debug_string);
}

ULONG BF_Osd_Attr_Get_Pc_Debug_Palette_Addr(ULONG* addr)
{
	if(addr) *addr = (ULONG)g_pc_debug_palette;
	return sizeof(g_pc_debug_palette);
}
#endif // DEBUG_ON_PC
