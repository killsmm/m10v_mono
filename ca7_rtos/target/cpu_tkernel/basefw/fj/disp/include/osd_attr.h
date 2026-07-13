/**
 * @file		osd_attr.h
 * @brief
 * @note		None
 * @attention	None
 *
 * <B><I>Copyright 2015 Socionext Inc.</I></B>
 */
#ifndef _OSD_ATTR_H_
#define _OSD_ATTR_H_

#include "osd_general.h"
#include "osd_draw_comm.h"

/*----------------------------------------------------------------------*/
/* Definition															*/
/*----------------------------------------------------------------------*/
#define CO_NOT_USE_DMA (1)
#define D_BF_OSD_ATTR_HDMI_INTERLACE	(1)		/**< Interlace */
#define D_BF_OSD_ATTR_HDMI_PROGRESSIVE	(0)		/**< Progressive */

/** Index of size array */
#define D_BF_OSD_ATTR_INDEX_WIDTH		(0)		/**< Width index of size array */
#define D_BF_OSD_ATTR_INDEX_LINES		(1)		/**< Lines index of size array */

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
#ifdef DEBUG_ON_PC
extern UCHAR g_pc_debug_main_y[];
extern UCHAR g_pc_debug_main_c[];
extern UCHAR g_pc_debug_osd[];
extern UCHAR g_pc_debug_work[];
extern UCHAR g_pc_debug_icon[];
extern UCHAR g_pc_debug_string[];
extern UCHAR g_pc_debug_palette[];
#endif // DEBUG_ON_PC

/*----------------------------------------------------------------------*/
/* Macro																*/
/*----------------------------------------------------------------------*/
// Nothing Special

/*----------------------------------------------------------------------*/
/* Function																*/
/*----------------------------------------------------------------------*/
/**
Get view freeze state.
@retval E_OSD_VIEW_FREEZE_OFF	unfreeze
@retval E_OSD_VIEW_FREEZE_ON	freeze
*/
extern E_OSD_VIEW_FREEZE BF_Osd_Attr_Get_View_Freeze(VOID);

/**
Set view to freeze or unfreeze.
@param[in] freeze freeze or unfreeze<br>
*/
extern VOID BF_Osd_Attr_Set_View_Freeze(E_OSD_VIEW_FREEZE freeze);

/**
Set display interface selection.
@param[in] disp_type	Display H/W type.<br>
						@ref FJ_DISP_TYPE
@param[in] ifs display interface selection<br>
			@ref E_FJ_DISP_IFS_NONE		: None<br>
			@ref E_FJ_DISP_IFS_LCD		: LCD<br>
			@ref E_FJ_DISP_IFS_HDMI		: HDMI<br>
			@ref E_FJ_DISP_IFS_MIPI		: MIPI<br>
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_NG				Abnormal End.
*/
extern FJ_ERR_CODE BF_Osd_Attr_Set_Display_Interface(FJ_DISP_TYPE disp_type, E_FJ_DISP_IFS ifs);

/**
Get display interface selection.
@param[in] disp_type	Display H/W type.<br>
						@ref FJ_DISP_TYPE
@retval @ref E_FJ_DISP_IFS_NONE	None
@retval @ref E_FJ_DISP_IFS_LCD	LCD
@retval @ref E_FJ_DISP_IFS_HDMI	HDMI
@retval @ref E_FJ_DISP_IFS_MIPI	MIPI
*/
extern E_FJ_DISP_IFS BF_Osd_Attr_Get_Display_Interface(FJ_DISP_TYPE disp_type);

/**
Get input image size of HDch for Base F/W APL.
@param[out]	input_size Input image size of HDch for Base F/W APL.
@retval FJ_ERR_OK				Normal End.
@retval FJ_ERR_OSD_SEQUENCE		Abnormal End. Sequence error.(Must be connect HDMI)
@retval FJ_ERR_NG				Abnormal End.
@note	The size of the image that the BaseF/W application should prepare for HDch is returned.<br>
		The value is different from a set value of the input size of the HIF macro. 
*/
extern FJ_ERR_CODE BF_Osd_Attr_Get_HDch_Input_Size(T_IMAGE_SIZE* input_size);

/**
Get scan mode(Interlcae/Progressive) of HDMI.
@retval D_BF_OSD_ATTR_HDMI_INTERLACE	interlace.
@retval other							progressive.
@attention	HDMI should be connecting
*/
extern BYTE BF_Osd_Attr_Get_HDMI_Scan_Mode(VOID);

/**
Get screen size of every layer.
@param[in]	bdisplayid layer ID.
@param[out]	screen_size screen size. [0]:width, [1]:height
*/
extern VOID BF_Osd_Attr_Get_Screen_Size(E_OSDLayer bdisplayid, USHORT screen_size[2]);

/**
Get draw area size.
@param[in]	disp_no_data GR number and block number.
@param[out]	screen_size screen size. [0]:width, [1]:height
*/
extern VOID BF_Osd_Attr_Get_Draw_Area_Size(const T_BF_SDRAM_MAP_COMMON_OSD* disp_no_data, USHORT screen_size[2]);

#ifdef DEBUG_ON_PC
/**
Get main address for pc debug.
@param[out] y_addr	Y address.<br>
@param[out] c_addr	CbCr address.<br>
*/
extern VOID BF_Osd_Attr_Get_Pc_Debug_Main_Addr(ULONG* y_addr, ULONG* c_addr);

/**
Get osd area address for pc debug.
@param[out] addr	OSD area address.<br>
@return		OSD area size.
*/
extern ULONG BF_Osd_Attr_Get_Pc_Debug_Osd_Addr(ULONG* addr);

/**
Get work area address for pc debug.
@param[out] addr	Work area address.<br>
@return		Work area size.
*/
extern ULONG BF_Osd_Attr_Get_Pc_Debug_Work_Addr(ULONG* addr);

/**
Get icon buffer address for pc debug.
@param[out] addr	Icon buffer address.<br>
@return		Icon buffer size.
*/
extern ULONG BF_Osd_Attr_Get_Pc_Debug_Icon_Addr(ULONG* addr);

/**
Get string buffer address for pc debug.
@param[out] addr	String buffer address.<br>
@return		String buffer size.
*/
extern ULONG BF_Osd_Attr_Get_Pc_Debug_String_Addr(ULONG* addr);

/**
Get palette buffer address for pc debug.
@param[out] addr	Palette buffer address.<br>
@return		Palette buffer size.
*/
extern ULONG BF_Osd_Attr_Get_Pc_Debug_Palette_Addr(ULONG* addr);
#endif // DEBUG_ON_PC

#endif // _OSD_ATTR_H_
