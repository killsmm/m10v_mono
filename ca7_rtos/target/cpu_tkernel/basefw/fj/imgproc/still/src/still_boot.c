/**
 * @file		still_boot.c
 * @brief		capture attribute for boot
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


// ****************************************************************************
//                          Include
// ****************************************************************************
#include "still_attr.h"

// ****************************************************************************
//                          Define
// ****************************************************************************
//#define CO_STILL_BOOT_DEBUG_ON

// ****************************************************************************
//                         Macro Define
// ****************************************************************************


// ****************************************************************************
//                           Global Data
// ****************************************************************************


// ****************************************************************************
//                           Local Data
// ****************************************************************************
static volatile T_CAPTURE_ATTR_FIRST gCap_Attr_First = {
	/* size_cap */
	.size_cap = {
		.width = 4608,
		.lines = 3456,
	},
	.size_screen_vga = {
		.width = 640,
		.lines = 480,
	},
	.size_screen_hd = {
		.width = 1440,
		.lines = 1080,
	},
	.cr_main = FJ_CR_4_5,
	.cr_screen_vga = FJ_CR_7_0,
	.cr_screen_hd = FJ_CR_7_0,
	.screen_vga_enable = 0x0000,
	.screen_hd_enable = 0x0000,
	.jpg_threshold = 0,
	.shoot_format = E_SHOOT_FORMAT_JPEG,
};

// ****************************************************************************
//                           Extern Data
// ****************************************************************************


// ****************************************************************************
//                    Local Method definition
// ****************************************************************************


// ****************************************************************************
//                    Local Method function
// ****************************************************************************


// ****************************************************************************
//                    Global Method function
// ****************************************************************************

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/ size_cap
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

USHORT BF_Still_Attr_Get_Size_Cap_Width(VOID)
{
	return gCap_Attr_First.size_cap.width;
}

USHORT BF_Still_Attr_Get_Size_Cap_Lines(VOID)
{
	return gCap_Attr_First.size_cap.lines;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/ size_screen_vga
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
VOID BF_Still_Attr_Set_Size_Screen_Vga_Width(USHORT width)
{
	gCap_Attr_First.size_screen_vga.width = width;
}

VOID BF_Still_Attr_Set_Size_Screen_Vga_Lines(USHORT lines)
{
	gCap_Attr_First.size_screen_vga.lines = lines;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/ size_screen_hd
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
VOID BF_Still_Attr_Set_Size_Screen_Hd_Width(USHORT width)
{
	gCap_Attr_First.size_screen_hd.width = width;
}

VOID BF_Still_Attr_Set_Size_Screen_Hd_Lines(USHORT lines)
{
	gCap_Attr_First.size_screen_hd.lines = lines;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/ cr_main
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
FJ_COMPRESSION_RATIO BF_Still_Attr_Get_Comp_Main(VOID)
{
	return gCap_Attr_First.cr_main;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/ cr_screen_vga
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
VOID BF_Still_Attr_Set_Comp_Screen_Vga(FJ_COMPRESSION_RATIO ratio)
{
	gCap_Attr_First.cr_screen_vga = ratio;
}

FJ_COMPRESSION_RATIO BF_Still_Attr_Get_Comp_Screen_Vga(VOID)
{
	return gCap_Attr_First.cr_screen_vga;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/ jpg_threshold
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
VOID BF_Still_Attr_Set_Jpeg_Threshold(USHORT threshold)
{
	gCap_Attr_First.jpg_threshold = threshold;
}

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//_/ shoot_image_format
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
E_SHOOT_FORMAT BF_Still_Attr_Get_Shoot_Image_Format(VOID)
{
	return gCap_Attr_First.shoot_format;
}

