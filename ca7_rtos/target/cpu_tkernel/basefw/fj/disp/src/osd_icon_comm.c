/**
 * @file		osd_icon_comm.c
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
#include "osd_icon_comm.h"
#include "osd_icon.h"
#include "osd_icon_cache.h"
#include "osd_attr.h"
//#include "decompress.h"
#include "sdram_map_cache_define.h"
#include "sdram_map_define.h"
#include "im_disp.h"
#include "fs_if.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define osd_icon_comm_set_rgba_palette_icon(icon_address)	for(hcnt = Height; hcnt>0; hcnt--){\
	for (wcnt = Width; wcnt>0; wcnt--) {\
	/* The ICON_RGBA data area of VGR_ICON_FORMAT is four byte boundary in each ICON. */\
	/* Therefore, SHORT can be accessed.*/\
		index = *(icon_address);\
		*(pWriteInfo) = iconInfo->rgba_palette_table[index];\
		icon_address++;\
		pWriteInfo++;\
	}\
	icon_address += iconLineFeed;\
	pWriteInfo += display_line_feed;\
}

#define  osd_icon_comm_blend_icon_directly(disp_block_no, iconAddr) if(color_replace_Fm != color_replace_To){ \
	for(j = icon_height; j>0; j--) {\
		for(i=icon_width; i>0; i--)	{\
			OSD_USHORT_INT iconCurPixel = *iconAddr;\
			osd_icon_comm_blend2pixel(disp_block_no, iconCurPixel,destAddr,alpha);\
			iconAddr ++;\
			destAddr ++;\
		}\
		iconAddr += iconLineFeed;\
		destAddr += display_line_feed;\
	}\
}\
else{\
	for(j = icon_height; j>0; j--){\
		for(i=icon_width; i>0; i--)	{\
			BF_Osd_Draw_Comm_Blend_Rgba(*iconAddr,destAddr, alpha);\
			iconAddr ++;\
			destAddr ++;\
		}\
		iconAddr += iconLineFeed;\
		destAddr += display_line_feed;\
	}\
}

#define osd_icon_made_blend_every_type_icon(scrpoint)  if(color_replace_Fm != color_replace_To){ \
	for(j = icon_height; j>0; j--) { \
		for(i=icon_width; i>0; i--) { \
			USHORT index = *(scrpoint); \
			OSD_USHORT_INT iconCurPixel = iconInfo->rgba_palette_table[index]; \
			if(iconCurPixel == color_replace_Fm){  /* Color replacement*/\
				iconCurPixel = color_replace_To;\
			}\
			BF_Osd_Draw_Comm_Blend_Rgba(iconCurPixel,destAddr,alpha);\
			scrpoint ++;\
			destAddr ++;\
		}\
		scrpoint += iconLineFeed;\
		destAddr += display_line_feed;\
	}\
}\
else{\
	for(j = icon_height; j>0; j--) {\
		for(i=icon_width; i>0; i--)	{\
			USHORT index = *(scrpoint);\
			OSD_USHORT_INT iconCurPixel = iconInfo->rgba_palette_table[index];\
			BF_Osd_Draw_Comm_Blend_Rgba(iconCurPixel,destAddr, alpha);\
			scrpoint ++;\
			destAddr ++;\
		}\
		scrpoint += iconLineFeed;\
		destAddr += display_line_feed;\
	}\
}
//Get uncompress icon data size. If the data's size format is 1/2,1/4 or 1/8 data, should be set the size to its integer times.
#define  osd_icon_comm_get_umcompress_size(W,H, num)	((((W) & ((num) - 1)) == 0) ? ((W) * (H)/(num)) : (((W)&(~((num)-1)))+(num)) * (H)/(num)) 
// In the case of 1 byte include 4 pixel
// X & 1= 0,1
// 1 - (0,1)= 1,0
// (3 - (0,1))*4= 4
// |7|6|5|4|3|2|1|0|
// |v|v|v|v|-|-|-|-|

// (3 - (0,1))*4= 0
// |7|6|5|4|3|2|1|0|
// |0|0|0|0|v|v|v|v|

// (3 - (0,1,2,3))*2= 6
// |7|6|5|4|3|2|1|0|
// |v|v|-|-|-|-|-|-|

// (3 - (0,1,2,3))*2= 4
// |7|6|5|4|3|2|1|0|
// |-|-|v|v|-|-|-|-|

// (3 - (0,1,2,3))*2= 2
// |7|6|5|4|3|2|1|0|
// |-|-|-|-|v|v|-|-|


// 3 - (0,1,2,3)= 0
// |7|6|5|4|3|2|1|0|
// |-|-|-|-|-|-|v|v|

// X & 3= 0,1,2,3
// 3 - (0,1,2,3)= 3,2,1,0

// 3 - (0,1,2,3)= 3
// |7|6|5|4|3|2|1|0|
// |v|v|-|-|-|-|-|-|

// 3 - (0,1,2,3)= 2
// |7|6|5|4|3|2|1|0|
// |-|-|v|v|-|-|-|-|

// 3 - (0,1,2,3)= 1
// |7|6|5|4|3|2|1|0|
// |-|-|-|-|v|v|-|-|


// 3 - (0,1,2,3)= 0
// |7|6|5|4|3|2|1|0|
// |-|-|-|-|-|-|v|v|

// In the case of 1 byte include 8 pixel
// X & 7= 0,1,2,3,4,5,6,7
// 7- (0,1,2,3,4,5,6)= 7
// |7|6|5|4|3|2|1|0|
// |v|-|-|-|-|-|-|-|
// 7- (0,1,2,3,4,5,6)= 6
// |7|6|5|4|3|2|1|0|
// |-|v|-|-|-|-|-|-|

// 7- (0,1,2,3,4,5,6)= 5
// |7|6|5|4|3|2|1|0|
// |-|-|v|-|-|-|-|-|

// 7- (0,1,2,3,4,5,6)= 4
// |7|6|5|4|3|2|1|0|
// |-|-|-|v|-|-|-|-|
//   :
// 7- (0,1,2,3,4,5,6)=1 
// |7|6|5|4|3|2|1|0|
// |-|-|-|-|-|-|v|-|

// 7- (0,1,2,3,4,5,6)=0 
// |7|6|5|4|3|2|1|0|
// |-|-|-|-|-|-|-|v|

#define osd_icon_comm_get_palette_index_from_bit(byte,bit,x,shift_bit_num_tmes,bit_pat)  (((byte) >> (((bit) -  ((x) & (bit)))<<shift_bit_num_tmes )) & (bit_pat)) 

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
// Kind of processed icon.
E_Draw_Type gBF_Osd_Icon_Font_ID = E_Draw_Type_OSD_Common_ICON;
E_FJ_OSD_ICON_BOOT gBF_Osd_Icon_Font_Boot_ID = E_FJ_OSD_ICON_COMM_GR;

//--- Resource data management ---
/*
	Load status of ICON data.
	FALSE : Not loaded.
	TRUE  : Loaded.
*/
static BOOL gBF_Osd_Icon_Is_Icon_Load[E_Draw_Type_End] = {
	TRUE,	// E_Draw_Type_OSD_Common_ICON
	TRUE,	// E_Draw_Type_OSD_Reload_ICON
	TRUE,	// E_Draw_Type_HDMI_Common_ICON
	TRUE	// E_Draw_Type_HDMI_Reload_ICON
};

/*
	Load status of String data.
	FALSE : Not loaded.
	TRUE  : Loaded.
*/
static BOOL gBF_Osd_Icon_Is_String_Load[E_Draw_Type_End] = {
	TRUE,	// E_Draw_Type_OSD_Common_ICON
	TRUE,	// E_Draw_Type_OSD_Reload_ICON
	TRUE,	// E_Draw_Type_HDMI_Common_ICON
	TRUE	// E_Draw_Type_HDMI_Reload_ICON
};

static BOOL gBF_Osd_Icon_Is_Icon_Load_boot[E_FJ_OSD_ICON_COMM_MAX] = {
	FALSE,
#if 0
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE
#endif
};

static BOOL gBF_Osd_Icon_Is_String_Load_boot[E_FJ_OSD_ICON_COMM_MAX] = {
	FALSE,
#if 0
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE
#endif
};

static UINT32 gBF_Osd_Icon_Boot_Addr[E_FJ_OSD_ICON_COMM_MAX] = {
	0xFFFFFFFF,
#if 0
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF
#endif
};

static UINT32 gBF_Osd_Icon_String_Boot_Addr[E_FJ_OSD_ICON_COMM_MAX] = {
	0xFFFFFFFF,
#if 0
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF
#endif
};

// Number of icon data
UINT32* gBF_Osd_Icon_Icon_Table_Num[E_Draw_Type_End] = {
#ifdef CO_DEBUG_ON_PC
	(UINT32*)g_pc_debug_icon,	// E_Draw_Type_VGR_Common_ICON
	(UINT32*)g_pc_debug_icon,	// E_Draw_Type_VGR_Reload_ICON
	(UINT32*)g_pc_debug_icon,	// E_Draw_Type_HGR_Common_ICON
	(UINT32*)g_pc_debug_icon	// E_Draw_Type_HGR_Reload_ICON
#else // CO_DEBUG_ON_PC

//	(UINT32*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(SDRAM_ADR_OSD_COMMON_ICON),			// E_Draw_Type_VGR_Common_ICON
//	(UINT32*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(SDRAM_ADR_OSD_RELOAD_ICON),			// E_Draw_Type_VGR_Reload_ICON
//	(UINT32*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(SDRAM_ADR_HDMI_COMMON_ICON),		// E_Draw_Type_HGR_Common_ICON
//	(UINT32*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(SDRAM_ADR_HDMI_RELOAD_ICON)			// E_Draw_Type_HGR_Reload_ICON
	(UINT32*)0,		// dummy
	(UINT32*)0,		// dummy
	(UINT32*)0,		// dummy
	(UINT32*)0		// dummy
#endif // CO_DEBUG_ON_PC
};

// Number of string data
UINT32* gBF_Osd_Icon_String_Table_Num[E_Draw_Type_End] = {
#ifdef CO_DEBUG_ON_PC
	(UINT32*)g_pc_debug_string,	// E_Draw_Type_VGR_Common_ICON
	(UINT32*)g_pc_debug_string,	// E_Draw_Type_VGR_Reload_ICON
	(UINT32*)g_pc_debug_string,	// E_Draw_Type_HGR_Common_ICON
	(UINT32*)g_pc_debug_string	// E_Draw_Type_HGR_Reload_ICON
#else // CO_DEBUG_ON_PC

//	(UINT32*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(SDRAM_ADR_OSD_COMMON_ICON_STRING),	// E_Draw_Type_VGR_Common_ICON
//	(UINT32*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(SDRAM_ADR_OSD_RELOAD_ICON_STRING),	// E_Draw_Type_VGR_Reload_ICON
//	(UINT32*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(SDRAM_ADR_HDMI_COMMON_ICON_STRING),	// E_Draw_Type_HGR_Common_ICON
//	(UINT32*)M_SDRAM_MAP_COMMON_ADR_PHY2LOG(SDRAM_ADR_HDMI_RELOAD_ICON_STRING)	// E_Draw_Type_HGR_Reload_ICON
	(UINT32*)0,		// dummy
	(UINT32*)0,		// dummy
	(UINT32*)0,		// dummy
	(UINT32*)0		// dummy
#endif // CO_DEBUG_ON_PC
};

// last icon's size that compressed
UINT32 gBF_Osd_Icon_Icon_Table_Last_offeset[E_Draw_Type_End] = {
	0, 0, 0, 0
};	// valid data num in Icon_Table

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Local Function														*/
/*----------------------------------------------------------------------*/
static VOID osd_icon_comm_blend2pixel(OSD_USHORT_INT block, OSD_USHORT_INT iconCurPixel,OSD_USHORT_INT* iconDest,BYTE alpha)
{
//	M_MBA_PRINTF(( D_MBA_KIND_FUNC_START, FJ_MBALOG_FUNC_ID_FUNC_START, FJ_MBALOG_TYPE_POINT, "2B:_Made_Blend2Pixel" ));

	// Color replacement
#if (FJ_OSD_COMM_RGBAXXXX == 2)
	OSD_USHORT_INT color_replace_Fm = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[block].color.replace[E_BF_OSD_DRAW_COMM_REPLACE_COLOR_FROM].word);
	OSD_USHORT_INT color_replace_To = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[block].color.replace[E_BF_OSD_DRAW_COMM_REPLACE_COLOR_TO].word);
#else
	OSD_USHORT_INT color_replace_Fm = (gBF_Osd_Draw_Comm_Osd_Attr[block].color.replace[E_BF_OSD_DRAW_COMM_REPLACE_COLOR_FROM].word);
	OSD_USHORT_INT color_replace_To = (gBF_Osd_Draw_Comm_Osd_Attr[block].color.replace[E_BF_OSD_DRAW_COMM_REPLACE_COLOR_TO].word);
#endif
	if(iconCurPixel == color_replace_Fm){
		iconCurPixel = color_replace_To;
	}
	BF_Osd_Draw_Comm_Blend_Rgba(iconCurPixel,iconDest,alpha);
}

/**
Information on the specified string is acquired.
@param[in]	a_Font_ID		String type.
@param[in]	a_string_id		String OS_USER_ID.
@param[out]	stringInfo		Information of string.
*/
static VOID bf_osd_icon_get_string_info(E_Draw_Type a_Font_ID, USHORT a_string_id, T_BF_OSD_ICON_COMM_STRING_INFO* stringInfo)
{
	T_BF_OSD_ICON_COMM_STRING_FILE_FMT* string_table;
	UINT32 dummy=0;

	// Get string information from string buffer.
	string_table = (T_BF_OSD_ICON_COMM_STRING_FILE_FMT*)BF_Osd_Icon_Get_String_Buffer_Addr(a_Font_ID, &dummy, TRUE);
	// Acquired information is stored in the output parameter.
	stringInfo->icon_id_num      = (BYTE)(string_table[a_string_id].icon_id_num);
	stringInfo->icon_id_list_ptr = (USHORT*)(((BYTE*)string_table) + string_table[a_string_id].string_data_offset);
}

/**
 *	@brief
 *	@param a_Font_ID
 *	@param a_string_id
 *	@param iconInfo
 *	@return     None
 *	@note		None
 *	@attention	None
 */
static VOID osd_icon_get_icon_info(E_Draw_Type a_Font_ID, USHORT icon_id, T_BF_OSD_ICON_COMM_ICON_INFO* iconInfo)
{
	T_BF_OSD_ICON_COMM_PARAMETER4CACHE get_icon_buffer;

	UINT32 maxsize_dumy = 0;

//	M_MBA_PRINTF(( D_MBA_KIND_FUNC_START, FJ_MBALOG_FUNC_ID_FUNC_START, FJ_MBALOG_TYPE_POINT, "2B:made_get_icon_inf" ));

	get_icon_buffer.iconList = 
				(T_BF_OSD_ICON_COMM_ICON_FILE_FMT*)BF_Osd_Icon_Get_Icon_Buffer_Addr(a_Font_ID, &maxsize_dumy, TRUE);
	get_icon_buffer.IconKind_id.short_data.iconID = icon_id;
	get_icon_buffer.IconKind_id.short_data.tableID = a_Font_ID;
	get_icon_buffer.Compress_Format_Icon_num.word = (*(gBF_Osd_Icon_Icon_Table_Num[a_Font_ID]));
	//Get Compressed Icon size
	if(get_icon_buffer.IconKind_id.short_data.iconID < get_icon_buffer.Compress_Format_Icon_num.Icon_Num_flag.Num -1) {	/* pgr0872 */
		//It's not last icon
		get_icon_buffer.compressed_size = get_icon_buffer.iconList[icon_id+1].mIcon_offset - get_icon_buffer.iconList[icon_id ].mIcon_offset;
	}
	else{
		//Last icon
		get_icon_buffer.compressed_size = gBF_Osd_Icon_Icon_Table_Last_offeset[a_Font_ID] - get_icon_buffer.iconList[icon_id].mIcon_offset;
	}

	BF_Osd_Icon_Comm_Set_Icon_Data(&get_icon_buffer, iconInfo);

}

///////////////////////////////////////////////////
//  Following are BASEFW_BOOT_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_BOOT_CODE"
/**
Set loading status of ICON data.
Loading the resource data of ICON is set.
@param[in]	font_id			ICON OS_USER_ID
@param[in]	state			Load status(TRUE: Loaded, FALSE: Not loaded)
*/
static VOID bf_osd_icon_set_icon_load_state(E_Draw_Type font_id, BOOL state)
{
	gBF_Osd_Icon_Is_Icon_Load[font_id] = state;
}


/**
Set loading status of string data.
Loading the resource data of ICON is set.
@param[in]	font_id			ICON OS_USER_ID
@param[in]	state			Load status(TRUE: Loaded, FALSE: Not loaded)
*/
static VOID bf_osd_icon_set_string_load_state(E_Draw_Type font_id, BOOL state)
{
	gBF_Osd_Icon_Is_String_Load[font_id] = state;
}

/*----------------------------------------------------------------------*/
/* Global Function														*/
/*----------------------------------------------------------------------*/
///////////////////////////////////////////////////
//  Following are BASEFW_COMM_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_COMM_CODE"
VOID BF_Osd_Icon_Comm_Draw_Icon_By_Bending(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no, 
	UINT32 global_width_display, 
	T_BF_OSD_ICON_COMM_DRAW_ICON_INFO const* const iconInfo,BYTE alpha,
	ULONG gr_Write_infor)
{
	if(alpha != D_BF_OSD_ICON_COMM_ALPHA_TRANS)
	{
		UINT32 i;
		UINT32 j;
		UINT32 icon_width = iconInfo->display_window.Width;
		UINT32 icon_height = iconInfo->display_window.Height;
		UINT32 iconLineFeed = iconInfo->icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] - icon_width;
		UINT32 display_line_feed = global_width_display - icon_width;
		OSD_USHORT_INT * destAddr = (OSD_USHORT_INT *)gr_Write_infor;
	#if (FJ_OSD_COMM_RGBAXXXX == 2)
		OSD_USHORT_INT color_replace_Fm = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].color.replace[E_BF_OSD_DRAW_COMM_REPLACE_COLOR_FROM].word);
		OSD_USHORT_INT color_replace_To = BF_Osd_General_TO_RGBA4444_FROM_RGBA8888(gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].color.replace[E_BF_OSD_DRAW_COMM_REPLACE_COLOR_TO].word);
	#else
		OSD_USHORT_INT color_replace_Fm = (gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].color.replace[E_BF_OSD_DRAW_COMM_REPLACE_COLOR_FROM].word);
		OSD_USHORT_INT color_replace_To = (gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].color.replace[E_BF_OSD_DRAW_COMM_REPLACE_COLOR_TO].word);
	#endif
		switch(iconInfo->icon_data.icon_format) {
			case E_BF_OSD_ICON_COMM_ICON_FORMAT_RGBA:{
				OSD_USHORT_INT * iconAddr = (OSD_USHORT_INT *)iconInfo->icon_data.mBuffer;
				osd_icon_comm_blend_icon_directly(disp_block_no, iconAddr);
				break;
				}
			case E_BF_OSD_ICON_COMM_ICON_FORMAT_16BIT:{
				// RGBA index DATA(16bit)
#if (FJ_OSD_COMM_RGBAXXXX == 2)
				//The data is set to RGBA4444 data directly and not palette ID
				//because RGBA4444 data is 2 bytes that mean the size is same as the palette ID
				OSD_USHORT_INT * iconAddr = (OSD_USHORT_INT *)iconInfo->icon_data.mBuffer;
				osd_icon_comm_blend_icon_directly(disp_block_no, iconAddr);
#else
				USHORT* ushort_scrpoint = (USHORT*)(iconInfo->icon_data.mBuffer);
				osd_icon_made_blend_every_type_icon(ushort_scrpoint);
#endif
				break;
				}
			case E_BF_OSD_ICON_COMM_ICON_FORMAT_8BIT:{
					BYTE* byte_scrpoint = (BYTE*)(iconInfo->icon_data.mBuffer);
					osd_icon_made_blend_every_type_icon(byte_scrpoint);
				break;
				}
			case E_BF_OSD_ICON_COMM_ICON_FORMAT_1BIT: // RGBA index DATA(1bit)
			case E_BF_OSD_ICON_COMM_ICON_FORMAT_2BIT: // RGBA index DATA(2bit)
			case E_BF_OSD_ICON_COMM_ICON_FORMAT_4BIT: {// RGBA index DATA(4bit)
				USHORT IconWidth = iconInfo->icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH];
				BYTE* byte_scrpoint = (BYTE*)(iconInfo->icon_data.mBuffer);
				UINT32 iconCurPixel;
				UINT32 max_remainder;
				UINT32 shift_bit_num;
				UINT32 valid_bit_patten;
				UINT32 shift_bit_num_tmes;
				switch(iconInfo->icon_data.icon_format) {
					case E_BF_OSD_ICON_COMM_ICON_FORMAT_4BIT:
						max_remainder = 1;
						shift_bit_num =1;
						valid_bit_patten = 0x0f;
						shift_bit_num_tmes = 2;
						break;
					case E_BF_OSD_ICON_COMM_ICON_FORMAT_2BIT:
						max_remainder = 3;
						shift_bit_num = 2;
						valid_bit_patten = 0x03;
						shift_bit_num_tmes = 1;
						break;
					default://1 bit
						max_remainder = 7;
						shift_bit_num = 3;
						valid_bit_patten = 0x01;
						shift_bit_num_tmes = 0;
						break;
				}

				if ((IconWidth & max_remainder)) {
					// Set the width to even
					IconWidth &= (~max_remainder);
					IconWidth += (max_remainder + 1);
				}
				IconWidth >>= shift_bit_num;
				if(color_replace_Fm != color_replace_To){
					for (j=icon_height; j>0; j--) {
						for (i=0 ; i<icon_width ; i++) {
							BYTE index = *(byte_scrpoint + ((iconInfo->start_pos_is_odd + i) >> shift_bit_num));
							index = osd_icon_comm_get_palette_index_from_bit(index, max_remainder, (i + iconInfo->start_pos_is_odd), shift_bit_num_tmes, valid_bit_patten);
							iconCurPixel = iconInfo->rgba_palette_table[index];
							if(iconCurPixel == color_replace_Fm){
								iconCurPixel = color_replace_To;
							}
							BF_Osd_Draw_Comm_Blend_Rgba(iconCurPixel, destAddr,alpha);
							destAddr ++;
						}
						byte_scrpoint += IconWidth;
						destAddr += display_line_feed;
					}
				}
				else{
					for (j = icon_height; j>0; j--) {
						for (i=0 ; i<icon_width ; i++) {
							BYTE index = *(byte_scrpoint + ((iconInfo->start_pos_is_odd + i) >> shift_bit_num));
							index = osd_icon_comm_get_palette_index_from_bit(index,max_remainder, (i + iconInfo->start_pos_is_odd), shift_bit_num_tmes, valid_bit_patten);
							iconCurPixel = iconInfo->rgba_palette_table[index];
							BF_Osd_Draw_Comm_Blend_Rgba(iconCurPixel, destAddr,alpha);
							destAddr ++;
						}
						byte_scrpoint += IconWidth;
						destAddr += display_line_feed;
					}
				}
				break;
				}
		}
	}
}

BYTE BF_Osd_Icon_Comm_Recalculate_Icon_Addr(E_BF_OSD_ICON_COMM_ICON_FMT Font_ID, T_BF_OSD_ICON_COMM_DRAW_ICON_INFO* iconInfo)
{
	UINT32	temp;
	SHORT	rect_relation_pos_y;
	SHORT	rect_relation_pos_x;
	BYTE	ret;

	rect_relation_pos_y = iconInfo->display_window.Postion.StartY - iconInfo->string_iocn_pos.StartY;
	rect_relation_pos_x = iconInfo->display_window.Postion.StartX - iconInfo->string_iocn_pos.StartX;
	iconInfo->start_pos_is_odd = 
	ret = (BYTE)(rect_relation_pos_x & 1);

	INT32 data_width = iconInfo->icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] ;
	switch (Font_ID) {
		case E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_8bit:
			iconInfo->icon_data.mBuffer += (iconInfo->icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] * rect_relation_pos_y + rect_relation_pos_x);	// Get display data address
			break;

		case E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON:
			iconInfo->icon_data.mBuffer += ((iconInfo->icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] * rect_relation_pos_y + rect_relation_pos_x) * FJ_OSD_COMM_RGBAXXXX);	// Get display data address
			break;

		case E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_16bit:
			iconInfo->icon_data.mBuffer += ((iconInfo->icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] * rect_relation_pos_y + rect_relation_pos_x) * 2);	// Get display data address
			break;

		case E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_4bit:
			temp = (iconInfo->icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] *rect_relation_pos_y) >> 1;
			iconInfo->icon_data.mBuffer += (temp + (rect_relation_pos_x >> 1));	// Get display data address
			break;

		case E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_2bit:
			if((data_width & 3) > 0){
				//Every line's last data should be 0booXXXXXX (here X is dumy data)
				data_width &=(~3);
				data_width +=4;
			}
			temp = (data_width *rect_relation_pos_y + rect_relation_pos_x ) >> 2;
			iconInfo->icon_data.mBuffer += (temp );	// Get display data address
			iconInfo->start_pos_is_odd = rect_relation_pos_x & 3;
			break;

		case E_BF_OSD_ICON_COMM_ICON_FMT_VGR_ICON_1bit:
			if((data_width & 7) > 0){
				//Every line's last data should be 0boXXXXXXX (here X is dumy data)
				data_width &=(~7);
				data_width +=8;
			}
			temp = (data_width *rect_relation_pos_y + rect_relation_pos_x) >> 3;
			iconInfo->icon_data.mBuffer += (temp );	// Get display data address
			iconInfo->start_pos_is_odd = rect_relation_pos_x & 7;
			break;

		default:
			// DO NOTHING
			break;
	}
	return ret;
}

VOID BF_Osd_Icon_Comm_Set_Icon_Data(T_BF_OSD_ICON_COMM_PARAMETER4CACHE* get_icon_buffer, T_BF_OSD_ICON_COMM_ICON_INFO* iconInfo)
{
	USHORT a_icon_id = get_icon_buffer->IconKind_id.short_data.iconID;
	T_BF_OSD_ICON_COMM_ICON_FILE_FMT* iconList = get_icon_buffer->iconList;

//	M_MBA_PRINTF(( D_MBA_KIND_FUNC_START, FJ_MBALOG_FUNC_ID_FUNC_START, FJ_MBALOG_TYPE_POINT, "B:Comm_Set_Icon_Data" ));

	iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] = iconList[a_icon_id].mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH];
	iconInfo->icon_format = iconList[a_icon_id].mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES] & (E_BF_OSD_ICON_COMM_ICON_FORMAT_1BIT);
	iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES] = iconList[a_icon_id].mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES] & E_BF_OSD_ICON_COMM_ICON_FORMAT_NON;

	switch(get_icon_buffer->Compress_Format_Icon_num.Icon_Num_flag.Compress_Flag) {
		case E_BF_OSD_ICON_COMM_COMPRESS_FORMAT_NONE:
			//None compressed
			iconInfo->mBuffer = (CHAR*)iconList;
			iconInfo->mBuffer += (iconList[a_icon_id].mIcon_offset);
			break;

		default:
			//Now LZW only
			//Set uncompress data size
			switch(iconInfo->icon_format){
				case E_BF_OSD_ICON_COMM_ICON_FORMAT_RGBA: // speed write, Write Y data
					get_icon_buffer->uncompressed_size = iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] * iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES] * FJ_OSD_COMM_RGBAXXXX;
					break;

				case E_BF_OSD_ICON_COMM_ICON_FORMAT_16BIT: // RGBA index DATA(16bit)
					get_icon_buffer->uncompressed_size = iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] * iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES] * 2;
					break;

				case E_BF_OSD_ICON_COMM_ICON_FORMAT_8BIT: // RGBA index DATA(8bit)
					get_icon_buffer->uncompressed_size = iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] * iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES] ;
					break;

				case E_BF_OSD_ICON_COMM_ICON_FORMAT_4BIT: // RGBA index DATA(4bit)
					get_icon_buffer->uncompressed_size = osd_icon_comm_get_umcompress_size(
											iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH],
											iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES], 2);
					break;

				case E_BF_OSD_ICON_COMM_ICON_FORMAT_2BIT: // RGBA index DATA(2bit)
					get_icon_buffer->uncompressed_size = osd_icon_comm_get_umcompress_size(
											iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH],
											iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES], 4);
					break;

				default:
					get_icon_buffer->uncompressed_size = osd_icon_comm_get_umcompress_size(
											iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH],
											iconInfo->mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES], 8);
					break;
			}
			if(get_icon_buffer->uncompressed_size & 3){
				//ensure 4 byte alignment
				get_icon_buffer->uncompressed_size -= (get_icon_buffer->uncompressed_size & 3);
				get_icon_buffer->uncompressed_size += 4;
			}

			iconInfo->mBuffer = BF_Osd_Icon_Cache_Get_icon(get_icon_buffer);
			break;
	}
}

UINT16 BF_Osd_Icon_Comm_Conv_Ascii_To_Icon_ID(UCHAR ascii)
{
	UINT16 ret = 0xffff;
	switch (ascii) {
		case '!':
			ret = D_BF_OSD_ICON_COMM_STR_ID_SURPRISE_MARK;
			break;

		case '&':
			ret = D_BF_OSD_ICON_COMM_STR_ID_AND_MARK;
			break;

		case 0x27: //'
			ret = D_BF_OSD_ICON_COMM_STR_ID_DASH_MARK;
			break;

		case '(':
			ret = D_BF_OSD_ICON_COMM_STR_ID_LEFT_PARENTHESIS;
			break;

		case ')':
			ret = D_BF_OSD_ICON_COMM_STR_ID_RIGHT_PARENTHESIS;
			break;

		case '*':
			ret = D_BF_OSD_ICON_COMM_STR_ID_CROSS_MARK;
			break;

		case '+':
			ret = D_BF_OSD_ICON_COMM_STR_ID_PLUS_MARK;
			break;

		case ',':
			ret = D_BF_OSD_ICON_COMM_STR_ID_COMMA_MARK;
			break;

		case '-':
			ret = D_BF_OSD_ICON_COMM_STR_ID_MINUS_MARK;
			break;

		case '.':
			ret = D_BF_OSD_ICON_COMM_STR_ID_DOT_MARK;
			break;

		case '/':
			ret = D_BF_OSD_ICON_COMM_STR_ID_SLASH_MARK;
			break;

		case ':':
			ret = D_BF_OSD_ICON_COMM_STR_ID_COLON_MARK;
			break;

		case '?':
			ret = D_BF_OSD_ICON_COMM_STR_ID_QUESTION_MARK;
			break;

		case '_':
			ret = D_BF_OSD_ICON_COMM_STR_ID_UNDERLINE_MARK;
			break;

		case ' ':
			ret = D_BF_OSD_ICON_COMM_STR_ID_SPACE_MARK;
			break;

		default:
			if ((ascii <= '9') && (ascii >= '0')) {
				// 0~9
				ret = (ascii - '0') + D_BF_OSD_ICON_COMM_STR_ID_NUM_ZERO;
			}
			else if ((ascii <= 'Z') && (ascii >= 'A')) {
				ret = (ascii - 'A') + D_BF_OSD_ICON_COMM_STR_ID_UPPER_A;
			}
			else if ((ascii <= 'z') && (ascii >= 'a')) {
				ret = (ascii - 'a') + D_BF_OSD_ICON_COMM_STR_ID_LOW_A;
			}
			else {
				// DO NOTHING
			}
			break;
	}
	return ret;
}

VOID BF_Osd_Icon_Comm_Clear_Resource_Load_State(VOID)
{
	BF_Osd_Icon_Clear_Resource_Load_State();
}


VOID BF_Osd_Icon_Comm_Copy_Box_To_Box(T_BF_OSD_ICON_COMM_DRAW_ICON_INFO const* const iconInfo, UINT32 DspWidth, ULONG WriteInfo)
{
	USHORT	wcnt;
	USHORT	hcnt;
	USHORT	Width = iconInfo->display_window.Width;
	USHORT	Height = iconInfo->display_window.Height;
	USHORT	IconWidth =iconInfo->icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH];
	USHORT	index;
	BYTE*	scrpoint = (BYTE*)(iconInfo->icon_data.mBuffer);
    BYTE   start_pos_is_odd = iconInfo->start_pos_is_odd;
	UINT32 iconLineFeed = iconInfo->icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] - Width;
	UINT32 display_line_feed = DspWidth - Width;

	OSD_USHORT_INT* pWriteInfo = (OSD_USHORT_INT*)(WriteInfo);

	switch (iconInfo->icon_data.icon_format) {
		case E_BF_OSD_ICON_COMM_ICON_FORMAT_RGBA: // speed write, Write Y data
			BF_Osd_General_Copy_Box_To_Box((ULONG)iconInfo->icon_data.mBuffer,
							iconInfo->icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] * FJ_OSD_COMM_RGBAXXXX,
							DspWidth * FJ_OSD_COMM_RGBAXXXX, WriteInfo, Width * FJ_OSD_COMM_RGBAXXXX, Height);

			break;

		case E_BF_OSD_ICON_COMM_ICON_FORMAT_16BIT: {// RGBA index DATA(16bit) 
#if (FJ_OSD_COMM_RGBAXXXX == 4) 
			//All icon data size is ensured that can be dividable 4 bytes alliment 
			USHORT* ushort_scrpoint = (USHORT*)(scrpoint); 
			//RGBA8888 
			osd_icon_comm_set_rgba_palette_icon(ushort_scrpoint); 
#else 
			//The data is set to RGBA4444 data directly and not palette ID 
			//because RGBA4444 data is 2 bytes that mean the size is same as the palette ID 
			BF_Osd_General_Copy_Box_To_Box((ULONG)iconInfo->icon_data.mBuffer, 
							iconInfo->icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] * FJ_OSD_COMM_RGBAXXXX, 
							DspWidth * FJ_OSD_COMM_RGBAXXXX, WriteInfo, Width * FJ_OSD_COMM_RGBAXXXX, Height); 
#endif
			}
			break;

		case E_BF_OSD_ICON_COMM_ICON_FORMAT_8BIT: // RGBA index DATA(8bit)
			osd_icon_comm_set_rgba_palette_icon(scrpoint);
			break;

		case E_BF_OSD_ICON_COMM_ICON_FORMAT_1BIT: // RGBA index DATA(1bit)
		case E_BF_OSD_ICON_COMM_ICON_FORMAT_2BIT: // RGBA index DATA(2bit)
		case E_BF_OSD_ICON_COMM_ICON_FORMAT_4BIT: {// RGBA index DATA(4bit)
				UINT32 max_remainder;
				UINT32 shift_bit_num;
				UINT32 valid_bit_patten;
				UINT32 shift_bit_num_tmes;
				switch(iconInfo->icon_data.icon_format) {
					case E_BF_OSD_ICON_COMM_ICON_FORMAT_4BIT:
						max_remainder = 1;//There are 2 pixels in 1 byte. x/2's mas remainder is 1
						shift_bit_num =1;//
						valid_bit_patten = 0x0f;
						shift_bit_num_tmes = 2;
						break;
					case E_BF_OSD_ICON_COMM_ICON_FORMAT_2BIT:
						max_remainder = 3;//There are 4 pixels in 1 byte. x/4's mas remainder is 3
						shift_bit_num = 2;
						valid_bit_patten = 0x03;
						shift_bit_num_tmes = 1;
						break;
					default://1 bit
						max_remainder = 7;//There are 8 pixels in 1 byte. x/8's mas remainder is 7
						shift_bit_num = 3;
						valid_bit_patten = 0x01;
						shift_bit_num_tmes = 0;
						break;
				}

				if ((IconWidth & max_remainder)) {
					// Set the width to even
					IconWidth &= (~max_remainder);
					IconWidth += (max_remainder + 1);
				}
				IconWidth >>= shift_bit_num;

				for (hcnt=Height ; hcnt>0 ; hcnt--) {
					for (wcnt=0 ; wcnt<Width ; wcnt++) {
						index = *(scrpoint + ((start_pos_is_odd + wcnt) >> shift_bit_num));
						index = osd_icon_comm_get_palette_index_from_bit(index,max_remainder, (wcnt + iconInfo->start_pos_is_odd), shift_bit_num_tmes, valid_bit_patten);
						*(pWriteInfo) = iconInfo->rgba_palette_table[index];
						pWriteInfo++;
					}
					scrpoint += IconWidth;
					pWriteInfo += display_line_feed;
				}
			}
			break;

		default:
			//Do nothing
			break;
	}
}

INT32 BF_Osd_Icon_Calculate_String_Size(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no, E_Draw_Type font_id, USHORT const* const icon_id_list_ptr, INT32 icon_num,
			USHORT string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_END], T_BF_OSD_ICON_COMM_DRAW_ICON_INFO iconInfo[MAX_ICON_NUM_IN_1STRING])
{
	INT32	ret;
	UINT32	i; /* Loop variable*/
	USHORT	icon_id;

	ret = icon_num;
	icon_id = 0;
	string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] = 0;
	string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES] = 0;

	for (i = 0; i < icon_num; i++) {
		icon_id = *(icon_id_list_ptr + i);
		if (icon_id < (*(gBF_Osd_Icon_Icon_Table_Num[font_id]))) {
			osd_icon_get_icon_info(font_id, icon_id, &iconInfo[i].icon_data);
			if ((gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].text.text_size.StartX > 0) && (gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].text.text_size.StartY > 0)) {
				string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] += (gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].text.space_size.StartX + gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].text.text_size.StartX);
				string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES] = BF_Osd_General_Get_Max(string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES], gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].text.text_size.StartY);
			}
			else {
				string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] += (iconInfo[i].icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] + gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].text.space_size.StartX);
				string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES] = BF_Osd_General_Get_Max(string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES], iconInfo[i].icon_data.mWidth_Height[E_BF_OSD_ICON_COMM_ICON_SIZE_LINES] & D_BF_OSD_ICON_COMM_HEIGHT_MASK);
			}
		}
		else {
			ret = -1;
			break;
		}
	}
	string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_WIDTH] -= gBF_Osd_Draw_Comm_Osd_Attr[disp_block_no].text.space_size.StartX; // last space will be minus
	return ret;
}

SHORT BF_Osd_Icon_Get_String_Draw_Range(E_BF_SDRAM_MAP_COMMON_OSD_DISP_BLOCK disp_block_no,
										USHORT uwStringID, E_Draw_Type Font_ID, T_BF_OSD_ICON_COMM_DRAW_ICON_INFO iconInfo[MAX_ICON_NUM_IN_1STRING],
										USHORT string_size[E_BF_OSD_ICON_COMM_ICON_SIZE_END])
{
	T_BF_OSD_ICON_COMM_STRING_INFO stringInfo;
	INT32 icon_num = -1;

	if (uwStringID < *(gBF_Osd_Icon_String_Table_Num[Font_ID])) {
		bf_osd_icon_get_string_info(Font_ID, uwStringID, &stringInfo);

		// valid string OS_USER_ID
		// Calculate codinate range
		icon_num = BF_Osd_Icon_Calculate_String_Size(disp_block_no, Font_ID, stringInfo.icon_id_list_ptr,
						 BF_Osd_General_Get_Min(stringInfo.icon_id_num, MAX_ICON_NUM_IN_1STRING), string_size, iconInfo);
	}
	return icon_num;
}

///////////////////////////////////////////////////
//  Following are BASEFW_BOOT_CODE
///////////////////////////////////////////////////
//#pragma arm section code="BASEFW_BOOT_CODE"
BOOL BF_Osd_Icon_Is_Icon_Loaded(E_Draw_Type font_id)
{
	return gBF_Osd_Icon_Is_Icon_Load[font_id];
}

BOOL BF_Osd_Icon_Is_String_Loaded(E_Draw_Type font_id)
{
	return gBF_Osd_Icon_Is_String_Load[font_id];
}

BOOL BF_Osd_Icon_Is_Icon_Boot_Loaded(E_FJ_OSD_ICON_BOOT font_id)
{
	return gBF_Osd_Icon_Is_Icon_Load_boot[font_id];
}

BOOL BF_Osd_Icon_Is_String_Boot_Loaded(E_FJ_OSD_ICON_BOOT font_id)
{
	return gBF_Osd_Icon_Is_String_Load_boot[font_id];
}

VOID BF_Osd_Icon_Clear_Resource_Load_State(VOID)
{
	INT32 i;

	for (i = 0; i < E_Draw_Type_End; i++) {
		gBF_Osd_Icon_Is_Icon_Load[i] =
		gBF_Osd_Icon_Is_String_Load[i] = FALSE;
	}
}

FJ_ERR_CODE BF_Osd_Icon_Load_Icon_File(CHAR const* const szFontName, E_Draw_Type font_ID)
{
	FJ_ERR_CODE	ret    = FJ_ERR_NG;
	FW_INT32	fileno = 0;
	FW_ULONG	length = 0;
	UINT32		max_size = 0;

	BYTE* pFontIconBuffer = (BYTE*)BF_Osd_Icon_Get_Icon_Buffer_Addr(font_ID, &max_size, FALSE);

	if (font_ID < E_Draw_Type_End) {
		// File can load to Font area
#ifdef DEBUG_ON_PC
		FILE *fp = fopen(szFontName, "rb");
		if (fp != NULL) {
			fseek(fp, 0, SEEK_END);
			fpos_t file_size;
			fgetpos(fp, &file_size);
			fseek(fp, 0, SEEK_SET);
			length = (FW_ULONG)file_size;
			if(length < max_size){
				fread(pFontIconBuffer, length, 1, fp);
				ret = FJ_ERR_OK; // OK
				bf_osd_icon_set_icon_load_state(font_ID, TRUE);
				BF_Osd_Icon_Cache_Clear_Nodes(font_ID);
				gBF_Osd_Icon_Icon_Table_Last_offeset[font_ID] = length - sizeof(UINT32);
			}
			fclose(fp);
		}
#else // DEBUG_ON_PC
		fileno = BF_Fs_If_Open((FW_CHAR*)szFontName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_READONLY);
		if (fileno != 0) {
			*(gBF_Osd_Icon_Icon_Table_Num[font_ID]) = 0; // Clear invalid
			if (BF_Fs_If_Read(fileno, pFontIconBuffer, max_size, &length) == 0) {
				if (length < max_size) {
					ret = FJ_ERR_OK; // OK
					bf_osd_icon_set_icon_load_state(font_ID, TRUE);
					//Clear current self icon/font cache
					BF_Osd_Icon_Cache_Clear_Nodes(font_ID);
					//Set last icon' end offset
					gBF_Osd_Icon_Icon_Table_Last_offeset[font_ID] = length - sizeof(UINT32);
				}
				else {
					// File size is too large.
					BF_Debug_Print_Error(("BF_Osd_Icon_Load_Icon_File(): [Error] File size error. length=%lu, max_size=%d\n", length, max_size));
					ret = FJ_ERR_OSD_FILE_SIZE;
				}
			}
			else {
				BF_Debug_Print_Error(("BF_Osd_Icon_Load_Icon_File(): [Error] File read error. fileno=%d\n", fileno));
				ret = FJ_ERR_OSD_FILE_ACCSESS;
			}
			BF_Fs_If_Close(fileno);
		}
#endif // DEBUG_ON_PC
		else {
			BF_Debug_Print_Error(("BF_Osd_Icon_Load_Icon_File(): [Error] File open error. fileno=%d\n", fileno));
			ret = FJ_ERR_OSD_FILE_ACCSESS;
		}
	}
	else {
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	return ret;
}

FJ_ERR_CODE BF_Osd_Icon_Load_String_File(CHAR const* const string_table_file, E_Draw_Type icon_fontID)
{
	FJ_ERR_CODE	ret    = FJ_ERR_NG;
	FW_INT32	fileno = 0;
	FW_ULONG	length = 0;
	UINT32		buffer_size = 0;
	BYTE*		p_string_buffer;

	if (icon_fontID < E_Draw_Type_End) {
		// ICON type is normal.

		// Get buffer address and size for the string data.
		p_string_buffer = (BYTE*)BF_Osd_Icon_Get_String_Buffer_Addr(icon_fontID, &buffer_size, FALSE);

#ifdef DEBUG_ON_PC
		FILE *fp = fopen(string_table_file, "rb");
		if (fp != NULL) {
			fseek(fp, 0, SEEK_END);
			fpos_t file_size;
			fgetpos(fp, &file_size);
			fseek(fp, 0, SEEK_SET);
			length = (FW_ULONG)file_size;
			if(length < buffer_size){
				fread(p_string_buffer, length, 1, fp);
				ret = FJ_ERR_OK;
//				*(gBF_Osd_Icon_String_Table_Num[icon_fontID]) = 0;
				bf_osd_icon_set_string_load_state(icon_fontID, TRUE);
			}
			fclose(fp);
		}
#else // DEBUG_ON_PC
		// File open.
		fileno = BF_Fs_If_Open((FW_CHAR*)string_table_file, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_READONLY);

		if (fileno != 0) {
			// The file opening was normally processed.

			// Clear invalid string num
			*(gBF_Osd_Icon_String_Table_Num[icon_fontID]) = 0;

			if (BF_Fs_If_Read(fileno, p_string_buffer, buffer_size, &length) == 0) {
				// The file reading was normally processed.
				if (length < buffer_size) {
					// Loading the string file was normally processed.
					ret = FJ_ERR_OK;
					bf_osd_icon_set_string_load_state(icon_fontID, TRUE);
				}
				else {
					// File size error. File size is too large.
					BF_Debug_Print_Error(("BF_Osd_Icon_Load_String_File(): [Error] File size error. length=%lu, buffer_size=%d\n", length, buffer_size));
					ret = FJ_ERR_OSD_FILE_SIZE;
				}
			}
			else {
				// File read error.
				BF_Debug_Print_Error(("BF_Osd_Icon_Load_String_File(): [Error] File read error. fileno=%d\n", fileno));
				ret = FJ_ERR_OSD_FILE_ACCSESS;
			}
			BF_Fs_If_Close(fileno);
		}
#endif // DEBUG_ON_PC
		else {
			// File open error.
			BF_Debug_Print_Error(("BF_Osd_Icon_Load_String_File(): [Error] File open error. fileno=%d\n", fileno));
			ret = FJ_ERR_OSD_FILE_ACCSESS;
		}
	}
	else {
		// Parameter error. ICON type is illegal.
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	return ret;
}

FJ_ERR_CODE BF_Osd_Icon_Load_RGB_Palette_Table(const char* RGBPaletteTableName, E_Draw_Type icon_fontID)
{
	FJ_ERR_CODE ret = FJ_ERR_NG;
	FW_INT32 fileno = 0;
	FW_ULONG length = 0;
	UINT32	buffer_size = 0;
	BYTE*	p_palette_buffer;

	if (icon_fontID < E_Draw_Type_End) {
		// File can load to Font area
		p_palette_buffer = (BYTE*)BF_Osd_Icon_Get_Palette_Buffer_Addr(icon_fontID, &buffer_size, FALSE);

#ifdef DEBUG_ON_PC
		FILE *fp = fopen(RGBPaletteTableName, "rb");
		if (fp != NULL) {
			fseek(fp, 0, SEEK_END);
			fpos_t file_size;
			fgetpos(fp, &file_size);
			fseek(fp, 0, SEEK_SET);
			length = (FW_ULONG)file_size;
			if(length < buffer_size){
				fread(p_palette_buffer, length, 1, fp);
				ret = FJ_ERR_OK; // OK
			}
			fclose(fp);
		}
#else // DEBUG_ON_PC
		fileno = BF_Fs_If_Open((FW_CHAR*)RGBPaletteTableName, D_BF_FS_IF_O_RDONLY, D_BF_FS_IF_ATTR_READONLY);
		if (fileno != 0) {
			if (BF_Fs_If_Read(fileno, p_palette_buffer, buffer_size, &length) == 0) {
				if (length < buffer_size) {
					ret = FJ_ERR_OK; // OK
				}
				else {
					// File size is too large.
					BF_Debug_Print_Error(("BF_Osd_Icon_Load_RGB_Palette_Table(): [Error] File size error. length=%lu, buffer_size=%d,File=%s\n", length, buffer_size,RGBPaletteTableName));
					ret = FJ_ERR_OSD_FILE_SIZE;
				}
			}
			else {
				BF_Debug_Print_Error(("BF_Osd_Icon_Load_RGB_Palette_Table(): [Error] File read error. fileno=%d\n", fileno));
				ret = FJ_ERR_OSD_FILE_ACCSESS;
			}
			BF_Fs_If_Close(fileno);
		}
#endif // DEBUG_ON_PC
		else {
			BF_Debug_Print_Error(("BF_Osd_Icon_Load_RGB_Palette_Table(): [Error] File open error. fileno=%d\n", fileno));
			ret = FJ_ERR_OSD_FILE_ACCSESS;
		}
	}
	else {
		ret = FJ_ERR_NG_INPUT_PARAM;
	}
	return ret;
}

ULONG BF_Osd_Icon_Get_Icon_Buffer_Addr(E_Draw_Type icon_fontID, UINT32* max_size, BOOL if_exclude_size)
{
	ULONG ret = 0;

#ifdef DEBUG_ON_PC
	*max_size = (UINT32)BF_Osd_Attr_Get_Pc_Debug_Icon_Addr(&ret);
#else // DEBUG_ON_PC

	switch (gBF_Osd_Icon_Font_Boot_ID) {
		case E_FJ_OSD_ICON_COMM_GR:
			*max_size = 0x1F2C00;
			break;

#if 0
		case E_FJ_OSD_ICON_COMM_PREVIEW_MENU:
			*max_size = 0xB7C280;
			break;

		case E_FJ_OSD_ICON_COMM_PLAYBACK_MENU:
			*max_size = 0xB7C280;
			break;

		case E_FJ_OSD_ICON_COMM_MOVIE_MENU:
			*max_size = 0xB7C280;
			break;

		case E_FJ_OSD_ICON_COMM_SETUP_MENU:
			*max_size = 0xB7C280;
			break;
		case E_FJ_OSD_ICON_COMM_USB_MENU:
			*max_size = 0xB7C280;
			break;
		case E_FJ_OSD_ICON_COMM_MESSAGE_MENU:
			*max_size = 0xB7C280;
			break;
#endif
		default:
			break;
	}

	ret = gBF_Osd_Icon_Boot_Addr[gBF_Osd_Icon_Font_Boot_ID];

#endif // DEBUG_ON_PC
	if (if_exclude_size) {
		// That must exclude the top in which table size is
		(*max_size) -= sizeof(UINT32);	// Really is 2 ushort, but 2 byte is pad
		ret += sizeof(UINT32);			// Really is 2 ushort, but 2 byte is pad
	}

	// No need to flush This area is R/W by CPU and D-cash only
	return M_SDRAM_MAP_COMMON_ADR_PHY2LOG(ret);
}

ULONG BF_Osd_Icon_Get_String_Buffer_Addr(E_Draw_Type icon_fontID, UINT32* max_size, BOOL if_exclude_size)
{
	ULONG ret = 0; // Set its to font string buffer

#ifdef DEBUG_ON_PC
	*max_size = (UINT32)BF_Osd_Attr_Get_Pc_Debug_String_Addr(&ret);
#else // DEBUG_ON_PC

	switch (gBF_Osd_Icon_Font_Boot_ID) {
		case E_FJ_OSD_ICON_COMM_GR:
			*max_size = 0x960;
			break;

#if 0
		case E_FJ_OSD_ICON_COMM_PREVIEW_MENU:
			*max_size = 0x960;
			break;

		case E_FJ_OSD_ICON_COMM_PLAYBACK_MENU:
			*max_size = 0x960;
			break;

		case E_FJ_OSD_ICON_COMM_MOVIE_MENU:
			*max_size = 0x960;
			break;

		case E_FJ_OSD_ICON_COMM_SETUP_MENU:
			*max_size = 0x960;
			break;
		case E_FJ_OSD_ICON_COMM_USB_MENU:
			*max_size = 0x960;
			break;
		case E_FJ_OSD_ICON_COMM_MESSAGE_MENU:
			*max_size = 0x960;
			break;
#endif
		default:
			break;
	}

	ret = gBF_Osd_Icon_String_Boot_Addr[gBF_Osd_Icon_Font_Boot_ID];

#endif // DEBUG_ON_PC
	if (if_exclude_size) {
		// That must exclude the top in which table size is
		(*max_size) -= sizeof(UINT32);	// Really is 2 ushort, but 2 byte is pad
		ret += sizeof(UINT32);			// Really is 2 ushort, but 2 byte is pad
	}

	// No need to flush This area is R/W by CPU and D-cash only
	return M_SDRAM_MAP_COMMON_ADR_PHY2LOG(ret);
}

ULONG BF_Osd_Icon_Get_Palette_Buffer_Addr(E_Draw_Type icon_fontID, UINT32* max_size, BOOL if_exclude_size)
{
	ULONG ret = 0; // Set its to font palette buffer

#ifdef DEBUG_ON_PC
	*max_size = (UINT32)BF_Osd_Attr_Get_Pc_Debug_Palette_Addr(&ret);
#else // DEBUG_ON_PC

#if 0
	switch (icon_fontID) {
		case  E_Draw_Type_OSD_Common_ICON:
			ret = SDRAM_ADR_OSD_COMMON_ICON_PALETTE;
			*max_size = SDRAM_SIZ_OSD_COMMON_ICON_PALETTE;
			break;

		case E_Draw_Type_OSD_Reload_ICON:
			ret =  SDRAM_ADR_OSD_RELOAD_ICON_PALETTE;
			*max_size = SDRAM_SIZ_OSD_RELOAD_ICON_PALETTE;
			break;

		case E_Draw_Type_HDMI_Common_ICON:
			ret =  SDRAM_ADR_HDMI_COMMON_ICON_PALETTE;
			*max_size = SDRAM_SIZ_HDMI_COMMON_ICON_PALETTE;
			break;

		case E_Draw_Type_HDMI_Reload_ICON:
			ret =  SDRAM_ADR_HDMI_RELOAD_ICON_PALETTE;
			*max_size = SDRAM_SIZ_HDMI_RELOAD_ICON_PALETTE;
			break;

		default:
			break;
	}
#endif

#endif // DEBUG_ON_PC
	if (if_exclude_size) {
		// That must exclude the top in which table size is
		(*max_size) -= sizeof(UINT32);	// Really is 2 ushort, but 2 byte is pad
		ret += sizeof(UINT32);			// Really is 2 ushort, but 2 byte is pad
	}

	// No need to flush This area is R/W by CPU and D-cash only
	return M_SDRAM_MAP_COMMON_ADR_PHY2LOG(ret);
}

FJ_ERR_CODE BF_Osd_Icon_Load_Icon_File_Boot( const E_FJ_OSD_ICON_BOOT type, UINT32 addr )
{
	gBF_Osd_Icon_Boot_Addr[type] = addr;
	gBF_Osd_Icon_Is_Icon_Load_boot[type] = TRUE;

	return FJ_ERR_OK;
}

FJ_ERR_CODE BF_Osd_Icon_Load_Icon_String_Boot( const E_FJ_OSD_ICON_BOOT type, UINT32 addr )
{
	gBF_Osd_Icon_String_Boot_Addr[type] = addr;
	gBF_Osd_Icon_Is_String_Load_boot[type] = TRUE;

	return FJ_ERR_OK;
}

VOID BF_Osd_Icon_Config( VOID )
{
	E_Draw_Type		icon_fontID = 0;
	UINT32			max_size;
	UINT32			icon_buff_addr = 0;
	UINT32			icon_data_addr_0_0 = 0;
	UINT32			icon_data_addr_0_1 = 0;
	UINT32			icon_data_addr_1 = 0;
	UINT32			osd_addr_0_0 = 0;
	UINT32			osd_addr_0_1 = 0;
	UINT32			osd_addr_1 = 0;
	UINT32			icon_data_size_64_64 = 64 * 64 * FJ_OSD_COMM_RGBAXXXX;
	UINT32			icon_data_size_64_32 = 64 * 32 * FJ_OSD_COMM_RGBAXXXX;
	
	T_BF_OSD_ICON_COMM_ICON_FILE_FMT* iconList;
	
	icon_buff_addr = BF_Osd_Icon_Get_Icon_Buffer_Addr(icon_fontID, &max_size, TRUE);
	if (icon_buff_addr == 0xFFFFFFFF) {
		return;
	}
	
	iconList = (T_BF_OSD_ICON_COMM_ICON_FILE_FMT*)(icon_buff_addr);
	
	// Icon data address
	icon_data_addr_0_0 = icon_buff_addr + iconList[D_BF_OSD_ICON_ID_CAPTURE].mIcon_offset;
	icon_data_addr_0_1 = icon_buff_addr + iconList[D_BF_OSD_ICON_ID_VIDEO].mIcon_offset;
	icon_data_addr_1 = icon_buff_addr + iconList[D_BF_OSD_ICON_ID_RECORDING].mIcon_offset;
	
	// OSD Area address
	osd_addr_0_0 = SDRAM_ADR_HOST_HDMI_OSD_FRAME_DATA_AREA_0;
	osd_addr_0_1 = osd_addr_0_0 + icon_data_size_64_64;
	osd_addr_1   = osd_addr_0_1 + icon_data_size_64_64;
	
	memcpy((VOID*)osd_addr_0_0, (VOID*)icon_data_addr_0_0, icon_data_size_64_64);
	memcpy((VOID*)osd_addr_0_1, (VOID*)icon_data_addr_0_1, icon_data_size_64_64);
	memcpy((VOID*)osd_addr_1, (VOID*)icon_data_addr_1, icon_data_size_64_32);
	
	// set OSD Config
	T_IM_DISP_CTRL_OSD_LAYER osd_ctrl;
	osd_ctrl.gridt.word	= 0;
	osd_ctrl.grtisize.size.width	= 960;
	osd_ctrl.grtisize.size.lines	= 540;
	osd_ctrl.grtdsta.word			= 0;
	osd_ctrl.gripo.bit.IPO1			= D_IM_DISP_IPO_RGBA8888_R;
	osd_ctrl.gripo.bit.IPO2			= D_IM_DISP_IPO_RGBA8888_G;
	osd_ctrl.gripo.bit.IPO3			= D_IM_DISP_IPO_RGBA8888_B;
	osd_ctrl.gripo.bit.IPO4			= D_IM_DISP_IPO_RGBA8888_A;
	osd_ctrl.grscctl_idset			= E_IM_DISP_INPUT_DATA_AREA_0;
	osd_ctrl.grisize[0].size.width	= 64;
	osd_ctrl.grisize[0].size.lines	= 64;
	osd_ctrl.grisize[1].size.width	= 64;
	osd_ctrl.grisize[1].size.lines	= 32;
	for (int index = 2; index < D_IM_DISP_OSD_DISPLAY_AREA_COUNT; index++) {
		osd_ctrl.grisize[index].size.width	= 16;
		osd_ctrl.grisize[index].size.lines	= 8;
	}
	osd_ctrl.grsa0[0]				= osd_addr_0_0;
	osd_ctrl.grsa0[1]				= osd_addr_0_1;
	osd_ctrl.grsa0[2]				= 0;
	osd_ctrl.grsa0[3]				= 0;
	osd_ctrl.grsa[0]				= osd_addr_1;
	for (int index = 1; index < D_IM_DISP_OSD_ADDR_BANK_SIZE; index++) {
		osd_ctrl.grsa[index]		= 0;
	}
	for (int index = 0; index < D_IM_DISP_OSD_DISPLAY_AREA_COUNT; index++) {
		osd_ctrl.grasa[index]		= 0;
	}
	osd_ctrl.grhga[0]				= 256;
	osd_ctrl.grhga[1]				= 256;
	for (int index = 2; index < D_IM_DISP_OSD_DISPLAY_AREA_COUNT; index++) {
		osd_ctrl.grhga[index]		= 16;
	}
	for (int index = 0; index < D_IM_DISP_OSD_DISPLAY_AREA_COUNT; index++) {
		osd_ctrl.grahga[index]		= 16;
	}
	osd_ctrl.grdsta[0].bit.DSH		= 50;
	osd_ctrl.grdsta[0].bit.DSV		= 50;
	osd_ctrl.grdsta[1].bit.DSH		= 150;
	osd_ctrl.grdsta[1].bit.DSV		= 68;
	for (int index = 2; index < D_IM_DISP_OSD_DISPLAY_AREA_COUNT; index++) {
		osd_ctrl.grdsta[index].bit.DSH	= 200 + 20 * index;
		osd_ctrl.grdsta[index].bit.DSV	= 10;
	}
	osd_ctrl.graren					= 0x00000000;
	osd_ctrl.grbsl					= 0x00000000;
	for (int index = 0; index < 5; index++) {
		osd_ctrl.grblink.word[index]	= 0;
	}
	osd_ctrl.grbinit				= 0x00000000;
	osd_ctrl.grbitrg				= D_IM_DISP_BITRG_NON;
	osd_ctrl.grrsz0					= E_IM_DISP_RSZSL_PADDING_THINNING;
	osd_ctrl.grrsz1.bit.HRSZM		= 2;
	osd_ctrl.grrsz1.bit.HRSZN		= 1;
	osd_ctrl.grrsz1.bit.HRSZOF		= 0;
	osd_ctrl.grrsz2.bit.VRSZM		= 2;
	osd_ctrl.grrsz2.bit.VRSZN		= 1;
	osd_ctrl.grrsz2.bit.VRSZOF		= 0;
	osd_ctrl.grrsz3.bit.HCSTA		= 0;
	osd_ctrl.grrsz3.bit.VCSTA		= 0;
	osd_ctrl.gry2r[0].word			= 0;
	osd_ctrl.gry2r[1].word			= 0;
	osd_ctrl.gry2r[2].word			= 0;
	osd_ctrl.gralp					= 0;

	Im_DISP_Ctrl_OSD_Layer(E_IM_DISP_HDMI, E_IM_DISP_SEL_LAYER_OSD_0, NULL, &osd_ctrl);
}

VOID BF_Osd_Icon_Draw( const INT32 icon_type )
{
	ULONG	area_enable	= 0;
	UINT32	hdmi_layer	= E_IM_DISP_SEL_LAYER_NONE;
	
	Im_DISP_Get_OSD_Area_Enable(E_IM_DISP_HDMI, E_IM_DISP_SEL_LAYER_OSD_0, &area_enable);
	
	switch (icon_type) {
		case D_BF_OSD_ICON_ID_CAPTURE:
			Im_DISP_Set_OSD_Area0_Switch(E_IM_DISP_HDMI, E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_INPUT_DATA_AREA_0);
			area_enable = area_enable | 0x00000001;
			break;
		case D_BF_OSD_ICON_ID_VIDEO:
			Im_DISP_Set_OSD_Area0_Switch(E_IM_DISP_HDMI, E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_INPUT_DATA_AREA_1);
			area_enable = area_enable | 0x00000001;
			break;
		case D_BF_OSD_ICON_ID_RECORDING:
			area_enable = area_enable | 0x00000002;
			break;
		default:
			return;
	}
	
	// Enable
	Im_DISP_Set_OSD_Area_Enable(E_IM_DISP_HDMI, E_IM_DISP_SEL_LAYER_OSD_0, area_enable);
	
	// OSD Layer display
	Im_DISP_Get_Status(&hdmi_layer, NULL);
	if ((hdmi_layer & E_IM_DISP_SEL_LAYER_OSD_0) != E_IM_DISP_SEL_LAYER_OSD_0) {
		Im_DISP_Start(E_IM_DISP_SEL_LAYER_OSD_0, E_IM_DISP_SEL_LAYER_NONE, 0);
	}
}

VOID BF_Osd_Icon_Erase( const INT32 icon_type )
{
	ULONG	area_enable = 0;
	
	Im_DISP_Get_OSD_Area_Enable(E_IM_DISP_HDMI, E_IM_DISP_SEL_LAYER_OSD_0, &area_enable);
	// set rect
	switch (icon_type) {
		case D_BF_OSD_ICON_ID_CAPTURE:
		case D_BF_OSD_ICON_ID_VIDEO:
			area_enable = area_enable & 0x000003FE;
			Im_DISP_Set_OSD_Area_Enable(E_IM_DISP_HDMI, E_IM_DISP_SEL_LAYER_OSD_0, area_enable);
			break;
		case D_BF_OSD_ICON_ID_RECORDING:
			area_enable = area_enable & 0x000003FD;
			Im_DISP_Set_OSD_Area_Enable(E_IM_DISP_HDMI, E_IM_DISP_SEL_LAYER_OSD_0, area_enable);
			break;
		default:
			break;
	}
}

