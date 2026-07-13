/**
 * @file		sdram_map_common.c
 * @brief		SDRAM map for common
 * @note		None
 * @attention	None
 * 
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */


#include "sdram_map_common.h"

/**
 * common map structure on SDRAM
 */
typedef struct {
	ULONG vde[ D_SDRAM_MAP_COMMON_SDRAM_BNK_VDE ];
} T_SDRAM_MAP_COMMON;

/**
 * HD display buffer map on SDRAM
 */
static T_SDRAM_MAP_COMMON gSdram_Map_Common_HD = {
	{ 0 },
};
/**
 * common map on VDE
 */
static T_SDRAM_MAP_COMMON gSdram_Map_Common_VDE = {
	{ SDRAM_ADR_LCD_MAIN_0,		SDRAM_ADR_LCD_MAIN_1,	 SDRAM_ADR_LCD_MAIN_2 },
};
static INT32 gHDMI_HD_Index = 0;
/**
 * common index & address structure
 */
typedef struct {
	BYTE index;      /**< array index */
	ULONG last_addr; /**< last(previous) address */
} T_SDRAM_COMMON_CTL;

/**
 * common control structure
 */
typedef struct {
	T_SDRAM_COMMON_CTL vde_ctl;
} T_SDRAM_MAP_COMMON_INDEX;

/**
 * common control
 */
static T_SDRAM_MAP_COMMON_INDEX gSdram_Map_Common_Ctl = {
	{0, SDRAM_ADR_LCD_MAIN_0},
};


//------------------------------------------------------------------------------
// VDE
//------------------------------------------------------------------------------
/**
 * get VDE address
 * @return ULONG: address
 */
ULONG Sdram_Map_Common_Get_Vde(VOID)
{
	return gSdram_Map_Common_VDE.vde[gSdram_Map_Common_Ctl.vde_ctl.index];
}

/**
 * get VDE(Y) address
 * @return ULONG: address
 */
ULONG Sdram_Map_Common_Get_Vde_Y(VOID)
{
	return gSdram_Map_Common_VDE.vde[gSdram_Map_Common_Ctl.vde_ctl.index];
}

/**
 * get VDE(Cb) address
 * @return ULONG: address
 */
ULONG Sdram_Map_Common_Get_Vde_Cb(VOID)
{
#if 0	// ### begin MILB_Porting
	U_IM_DISP_SIZE	imgsiz;
	Im_DISP_Get_Input_Size(&imgsiz);
	UINT32 display_size = imgsiz.size.lines * imgsiz.size.width;  

	return (gSdram_Map_Common_VDE.vde[gSdram_Map_Common_Ctl.vde_ctl.index] + display_size);
#else	// ### end MILB_Porting
	return 0ul;
#endif	// ### end MILB_Porting
}

/**
 * change VDE address
 * @return ULONG: address
 */
ULONG Sdram_Map_Common_Change_Vde( BOOL update)
{
	ULONG addr[3];

	gSdram_Map_Common_Ctl.vde_ctl.last_addr = Sdram_Map_Common_Get_Vde();
	if (update) ++gSdram_Map_Common_Ctl.vde_ctl.index;
	gSdram_Map_Common_Ctl.vde_ctl.index = SDRAM_BNK_LIMIT(gSdram_Map_Common_Ctl.vde_ctl.index, D_SDRAM_MAP_COMMON_SDRAM_BNK_VDE);

	FJ_GetMainDispBuffAddr( E_OSDLayer_LCD_MAIN, addr );

	if (Sdram_Map_Common_Get_Vde() == addr[0]) {
		if (update) ++gSdram_Map_Common_Ctl.vde_ctl.index;
		gSdram_Map_Common_Ctl.vde_ctl.index = SDRAM_BNK_LIMIT(gSdram_Map_Common_Ctl.vde_ctl.index, D_SDRAM_MAP_COMMON_SDRAM_BNK_VDE);
	}
	return Sdram_Map_Common_Get_Vde();
}

/**
 * next VDE address
 * @return ULONG: address
 */
ULONG Sdram_Map_Common_Next_Vde(VOID)
{
	BYTE vram_index;
	
	vram_index = gSdram_Map_Common_Ctl.vde_ctl.index;
	++vram_index;
	vram_index = SDRAM_BNK_LIMIT(vram_index, D_SDRAM_MAP_COMMON_SDRAM_BNK_VDE);
	return gSdram_Map_Common_VDE.vde[vram_index];
}

/**
 * change HD Index
 */
ULONG Sdram_Map_Common_Change_HDMI_HD(VOID)
{
	gHDMI_HD_Index++;
	if( gHDMI_HD_Index > 2 ){
		gHDMI_HD_Index = 0;
	}
	return Sdram_Map_Common_Get_HDMI_HD();
}

/**
 * nect HD Index
 */
ULONG Sdram_Map_Common_Next_HDMI_HD(VOID)
{
	return Sdram_Map_Common_Get_HDMI_HD();
}

/**
 * Get HD address
 * @return ULONG: address
 */
ULONG Sdram_Map_Common_Get_HDMI_HD(VOID)
{
	return gSdram_Map_Common_HD.vde[gHDMI_HD_Index];
}

/**
 * Get HD(Y) address
 * @return ULONG: address
 */
ULONG Sdram_Map_Common_Get_HDMI_HD_Y(VOID)
{
	return gSdram_Map_Common_HD.vde[gHDMI_HD_Index];
}

/**
 * get HD(Cb) address
 * @return ULONG: address
 */
ULONG Sdram_Map_Common_Get_HDMI_HD_Cb(VOID)
{
#if 0	// ### begin MILB_Porting
	U_IM_DISP_SIZE	imgsiz;
	Im_DISP_Get_Input_Size(&imgsiz);
	UINT32 display_size = imgsiz.size.lines * imgsiz.size.width;  

	return (gSdram_Map_Common_HD.vde[gHDMI_HD_Index] + display_size);
#else	// ### end MILB_Porting
	return 0ul;
#endif	// ### end MILB_Porting
}

/**
 * get current HD Index by current addres
 */
INT32 Sdram_Map_Common_Get_HDMI_HD_Current_Index(VOID)
{
	return gHDMI_HD_Index;
}
